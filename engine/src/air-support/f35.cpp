#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "f35.hpp"
#include "../player.hpp"
#include "../direction.hpp"
#include "../constants.hpp"
#include "../npc/paths.hpp"
#include "../font.hpp"
#include "../colors.hpp"
#include "../rng.hpp"
#include "../sound/gunshot.hpp"
#include "../sound/npc.hpp"
#include "../viewport.hpp"
#include "../damage/explosions.hpp"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_mutex.h>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[AIR-SUPPORT][F35][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[AIR-SUPPORT][F35][ERROR]: " << A << "\n";

namespace air_support::f35 {
  static bool halt_f35 = false;
  static constexpr int AIR_SUPPORT_AUDIO_CHANNEL = 3;
  static constexpr std::size_t F35_WIDTH = 375;
  static constexpr std::size_t F35_HEIGHT = 260;
  static constexpr uint16_t FACE_SOUTH = 90;
  static constexpr uint16_t FACE_EAST = 0;
  static constexpr uint16_t MOVEMENT_AMOUNT = 80;
  static constexpr int AIRCRAFT_CARRIER_X_POSITION = -20800;
  static std::vector<SDL_Point> bomb_targets;
#define MODE_CAST_RAYS 1
#define MODE_SAVE_RAYS 2
#define MODE_DISPATCH_NOW 3
  static int mode = 0;
  static Mix_Chunk* flyover = nullptr;
  static constexpr std::size_t CONFIRMATION_SIZE = 3;
  static std::array<Mix_Chunk*,CONFIRMATION_SIZE> confirmation;
  static SDL_mutex* f35_list_mutex = SDL_CreateMutex();
  static SDL_mutex* bomb_targets_mutex = SDL_CreateMutex();
  static SDL_mutex* mode_mutex = SDL_CreateMutex();
  void move_map(int dir, int amount){
    if(halt_f35){
      return;
    }
    LOCK_MUTEX(bomb_targets_mutex);
    for(auto& wall : bomb_targets){
      switch(dir) {
        case NORTH_EAST:
          wall.y += amount;
          wall.x -= amount;
          break;
        case NORTH_WEST:
          wall.y += amount;
          wall.x += amount;
          break;
        case NORTH:
          wall.y += amount;
          break;
        case SOUTH_EAST:
          wall.y -= amount;
          wall.x -= amount;
          break;
        case SOUTH_WEST:
          wall.y -= amount;
          wall.x += amount;
          break;
        case SOUTH:
          wall.y -= amount;
          break;
        case WEST:
          wall.x += amount;
          break;
        case EAST:
          wall.x -= amount;
          break;
        default:
          break;
      }
    }
    UNLOCK_MUTEX(bomb_targets_mutex);
  }
void draw_target(SDL_Point p){
  if(halt_f35){
    return;
  }
  SDL_Rect r;
  r.x = p.x;
  r.y = p.y;
  r.w = 50;
  r.h = 50;
  draw::blatant_rect(&r);
}

int initial_x = 0;
int initial_y = 0;
std::vector<std::pair<int,int>> target_offsets = {
  {0,0},
  {125,-125},
  {400,-125},
  {125,125},
  {400,125},
  //{525,125},
  //{400,250},
};
static inline void cast_rays(){
  if(halt_f35){
    return;
  }
  const int& x = plr::cx();
  const int& y = plr::cy();
  for(const auto& p : target_offsets){
    draw_target({x + p.first, y + p.second});
  }
}
static inline std::vector<SDL_Point> save_bomb_targets(){
  if(halt_f35){
    return {};
  }
  m_debug("save_bomb_targets");
  const int& x = plr::cx();
  const int& y = plr::cy();
  std::vector<SDL_Point> list;
  for(const auto& p : target_offsets){
    list.emplace_back(x + p.first,y + p.second);
  }
  return list;
}
void F35::calculate_aim() {
  target_x = plr::get_cx();
  target_y = plr::get_cy();
}
void spawn(const int& in_start_x, const int& in_start_y) {
  if(halt_f35){
    return;
  }
  LOCK_MUTEX(f35_list_mutex);
  f35_list.emplace_front(in_start_x,in_start_y);
  UNLOCK_MUTEX(f35_list_mutex);
}
void position_fleet_at_aircraft_carrier(){
  if(halt_f35){
    return;
  }
  int start_x = AIRCRAFT_CARRIER_X_POSITION;
  int i=1;
  LOCK_MUTEX(f35_list_mutex);
  for(auto& jet : f35_list){
    jet.hp = 550;
    jet.max_hp = 850;
    jet.ready = true;

    jet.state_index = 0;
    jet.m_last_fire_tick = 0;
    jet.move_to(start_x + (-2080 * i),(i - 1) * F35_HEIGHT);
    jet.calc();
    jet.on_carrier = true;
    jet.m_dispatched = false;
    ++i;
  }
  UNLOCK_MUTEX(f35_list_mutex);
  m_debug("F35 squadron returned to aircraft carrier");
}
std::vector<SDL_Point> bomb_impacts;
bool accept_space_bar = true;
uint32_t dispatch_func(uint32_t interval, void* name){
  if(halt_f35){
    return 0;
  }
  LOCK_MUTEX(mode_mutex);
  mode = MODE_DISPATCH_NOW;
  UNLOCK_MUTEX(mode_mutex);
  std::size_t dispatched_jets = 0;
  std::size_t i = 0;
  LOCK_MUTEX(f35_list_mutex);
  for(auto& jet : f35_list){
    jet.call_in_airstrike(i);
    jet.m_dispatched = true;
    jet.on_carrier = false;
    //jet.randomize_bombing_targets();
    ++dispatched_jets;
    ++i;
  }
  UNLOCK_MUTEX(f35_list_mutex);
  return 0;
}
uint32_t reset_mode_func(uint32_t interval, void* ignored){
  if(halt_f35){
    return 0;
  }
  LOCK_MUTEX(mode_mutex);
  mode = MODE_CAST_RAYS;
  UNLOCK_MUTEX(mode_mutex);
  LOCK_MUTEX(bomb_targets_mutex);
  bomb_targets.clear();
  UNLOCK_MUTEX(bomb_targets_mutex);
  accept_space_bar = true;
  return 0;
}
uint32_t detonate_targets(uint32_t interval, void* ignored){
  if(halt_f35){
    return 0;
  }
  m_debug("detonate_targets");
  LOCK_MUTEX(bomb_targets_mutex);
  for(auto target : bomb_targets){
    damage::explosions::detonate_at(&target,rand_between(150,550),rand_between(500,850),rand_between(0,3));
  }
  UNLOCK_MUTEX(bomb_targets_mutex);
  if(interval == 3000){
    return 0;
  }
  return 3000;
}
uint32_t play_flyover(uint32_t interval, void* ignored){
  if(halt_f35){
    return 0;
  }
  m_debug("play_flyover");
  Mix_PlayChannel(AIR_SUPPORT_AUDIO_CHANNEL,flyover,0);
  return 0;
}
void init() {
  LOCK_MUTEX(f35_list_mutex);
  int start_x = AIRCRAFT_CARRIER_X_POSITION;
  for(int i=1; i <= 3; ++i){
    spawn(start_x + (-2080 * i), (i - 1) * F35_HEIGHT);
  }
  for(int i=1; i <= 3; ++i){
    spawn(start_x + (-3080 * i), (i - 1) * F35_HEIGHT);
  }
  flyover = Mix_LoadWAV("../assets/sound/airstrike/f35-flyover-0.wav");
  confirmation[0] = Mix_LoadWAV("../assets/sound/airstrike/airstrike-radio-0.wav");
  confirmation[1] = Mix_LoadWAV("../assets/sound/airstrike/airstrike-radio-1.wav");
  confirmation[2] = Mix_LoadWAV("../assets/sound/airstrike/airstrike-radio-2.wav");
  /** FIXME: change to idle */
  UNLOCK_MUTEX(f35_list_mutex);
  LOCK_MUTEX(mode_mutex);
  mode = MODE_CAST_RAYS;
  UNLOCK_MUTEX(mode_mutex);
}
void play_random_confirmation(){
  Mix_PlayChannel(AIR_SUPPORT_AUDIO_CHANNEL,confirmation[rng::next() % 3],0);
}
void space_bar_pressed(){
  if(halt_f35){
    return;
  }
  if(accept_space_bar){
    accept_space_bar = false;
  }else{
    return;
  }
  LOCK_MUTEX(bomb_targets_mutex);
  bomb_targets = save_bomb_targets();
  UNLOCK_MUTEX(bomb_targets_mutex);
  play_random_confirmation();
#ifdef EXPLOSION_DIVISOR
  SDL_AddTimer(5000 / EXPLOSION_DIVISOR,dispatch_func,const_cast<char*>(""));
  SDL_AddTimer(10000 / EXPLOSION_DIVISOR,play_flyover,const_cast<char*>(""));
  SDL_AddTimer(15000 / EXPLOSION_DIVISOR,detonate_targets,const_cast<char*>(""));
  SDL_AddTimer(40000 / EXPLOSION_DIVISOR,reset_mode_func,const_cast<char*>(""));
#else
  SDL_AddTimer(5000,dispatch_func,const_cast<char*>(""));
  SDL_AddTimer(10000,play_flyover,const_cast<char*>(""));
  SDL_AddTimer(15000,detonate_targets,const_cast<char*>(""));
  SDL_AddTimer(40000,reset_mode_func,const_cast<char*>(""));
#endif
  LOCK_MUTEX(mode_mutex);
  mode = MODE_SAVE_RAYS;
  UNLOCK_MUTEX(mode_mutex);
}
uint32_t F35::cooldown_between_shots(){
  return 1000;
}
bool F35::can_fire_again() {
  return m_last_fire_tick + cooldown_between_shots() <= tick::get();
}
bool F35::headed_back_to_base(){
  return m_dispatched == false || on_carrier;
}
void F35::move_to(const int32_t& x,const int32_t& y) {
  self.rect.x = x;
  self.rect.y = y;
}
void F35::perform_ai() {
  if(dispatched()){
    self.rect.x += MOVEMENT_AMOUNT;
    calc();
  }
}
bool F35::dispatched(){
  return on_carrier == false || m_dispatched;
}
void tick() {
  if(halt_f35){
    return;
  }
  static bool initial_set = false;
  if(!initial_set){
    initial_x = plr::cx();
    initial_y = plr::cy();
    initial_set = true;
  }
  if(mode == MODE_CAST_RAYS){
    cast_rays();
    return;
  }
  if(mode > MODE_CAST_RAYS && bomb_targets.size()){
    /**
     * TODO: display warning about airstrike
     */
    for(const auto& point : bomb_targets){
      draw_target(point);
    }
  }

  for(auto& s : f35_list) {
    if(!s.dispatched()){
      continue;
    }
    s.tick();
    SDL_RenderCopyEx(
        ren,  //renderer
        s.self.bmp[0].texture,
        nullptr,// src rect
        &s.self.rect,
        s.angle, // angle
        nullptr,  // center
        SDL_FLIP_NONE // flip
        );
  }
}
SDL_Texture* F35::initial_texture() {
  return self.bmp[0].texture;
}
void F35::calc() {
  cx = self.rect.x + self.rect.w / 2;
  cy = self.rect.y + self.rect.h / 2;
  angle = FACE_EAST;
}
void F35::tick() {
  perform_ai();
}
Asset* F35::next_state() {
  return states[0];
}
int F35::gun_damage() {
  return rand_between(550,850);
}

F35::F35() {
  ready = false;
  call_count = 0;
}
F35::F35(const int32_t& _x,
    const int32_t& _y){
  call_count = 0;
  self.rect.x = _x;
  self.rect.y = _y;
  self.rect.w = F35_WIDTH;
  self.rect.h = F35_HEIGHT;
  movement_amount = 450;
  self.load_bmp_asset(BMP);

  hp = 550;
  max_hp = 850;
  ready = true;

  m_dispatched = false;
  on_carrier = true;
  state_index = 0;
  calc();
  m_last_fire_tick = 0;
  move_to(_x,_y);
}
bool is_dispatched(){
  std::size_t dispatched_jets = 0;
  for(const auto& jet : f35_list){
    if(jet.m_dispatched){
      ++dispatched_jets;
    }
  }
  return dispatched_jets > 0;
}
void F35::randomize_bombing_targets(){
  //bombing_targets.clear();
  //for(size_t i=0; i < 8; i++){
  //  SDL_Point p;
  //  p.x = initial_x + 50;
  //  p.y = initial_y;
  //  draw::line(self.rect.x, self.rect.y,p.x,p.y);
  //  bombing_targets.emplace_back(p);
  //}
}
void return_to_carrier(){
  LOCK_MUTEX(f35_list_mutex);
  position_fleet_at_aircraft_carrier();
  for(auto& jet : f35_list){
    jet.m_dispatched = false;
    jet.on_carrier = true;
  }
  UNLOCK_MUTEX(f35_list_mutex);
}
void F35::call_in_airstrike(std::size_t i){
  const int& start_x = AIRCRAFT_CARRIER_X_POSITION;
  hp = 550;
  max_hp = 850;
  ready = true;

  state_index = 0;
  m_last_fire_tick = 0;
  move_to(start_x + (-2080 * i),(i - 1) * F35_HEIGHT);
  calc();
  on_carrier = true;
  m_dispatched = false;

}
void program_exit(){
  halt_f35 = true;
  bomb_targets.clear();
  Mix_FreeChunk(flyover);
  flyover = nullptr;
  for(size_t i=0; i < CONFIRMATION_SIZE;i++){
    if(confirmation[i] == nullptr){
      continue;
    }
    Mix_FreeChunk(confirmation[i]);
    confirmation[i] = nullptr;
  }
  for(auto& f : f35_list){
    f.bombing_targets.clear();
    f.self = {};
    f.states.clear();
  }
  f35_list.clear();
  bomb_impacts.clear();
}
};

#undef m_debug
