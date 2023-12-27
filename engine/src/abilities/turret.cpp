#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "turret.hpp"
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
#define m_debug(A) std::cout << "[AIR-SUPPORT][TURRET][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[AIR-SUPPORT][TURRET][ERROR]: " << A << "\n";
extern double angleBetweenPoints(double x1, double y1, double x2, double y2);

namespace abilities::turret {
  static bool halt_turret = false;
  static constexpr int TURRET_AUDIO_CHANNEL = 5;
  static constexpr std::size_t TURRET_WIDTH = 85;
  static constexpr std::size_t TURRET_HEIGHT = 130;
  static constexpr std::string_view BMP = "../assets/turret.bmp";
  static int mode = 0;
  static SDL_mutex* turret_list_mutex = SDL_CreateMutex();
  static SDL_mutex* mode_mutex = SDL_CreateMutex();
  static std::forward_list<std::unique_ptr<Turret>> turret_list;
  static constexpr int TURRET_IDLE = 0;
  static constexpr int TURRET_DEPLOY = 1;
  static constexpr int TURRET_HUNT = 2;
  static constexpr int TURRET_DESTROY = 3;

  void move_map(int dir, int amount){
    if(halt_turret){
      return;
    }
    LOCK_MUTEX(turret_list_mutex);
    for(auto& t : turret_list){
      switch(dir) {
        case NORTH_EAST:
          t->self.rect.y += amount;
          t->self.rect.x -= amount;
          break;
        case NORTH_WEST:
          t->self.rect.y += amount;
          t->self.rect.x += amount;
          break;
        case NORTH:
          t->self.rect.y += amount;
          break;
        case SOUTH_EAST:
          t->self.rect.y -= amount;
          t->self.rect.x -= amount;
          break;
        case SOUTH_WEST:
          t->self.rect.y -= amount;
          t->self.rect.x += amount;
          break;
        case SOUTH:
          t->self.rect.y -= amount;
          break;
        case WEST:
          t->self.rect.x += amount;
          break;
        case EAST:
          t->self.rect.x -= amount;
          break;
        default:
          break;
      }
    }
    UNLOCK_MUTEX(turret_list_mutex);
  }

  static inline void cast_rays(){
    if(halt_turret){
      return;
    }
    const int& x = plr::cx();
    const int& y = plr::cy();
    SDL_Rect r;
    r.x = x + 50;
    r.y = y + 50;
    r.w = 50;
    r.h = 50;
    draw::blatant_rect(&r);
  }
  void Turret::calculate_aim() {
    target_x = plr::get_cx();
    target_y = plr::get_cy();
  }
  void spawn(const int& in_start_x, const int& in_start_y) {
    if(halt_turret){
      return;
    }
    LOCK_MUTEX(turret_list_mutex);
    turret_list.emplace_front(std::make_unique<Turret>(in_start_x,in_start_y));
    UNLOCK_MUTEX(turret_list_mutex);
  }
  bool accept_space_bar = true;
  void init() {
    LOCK_MUTEX(mode_mutex);
    mode = TURRET_IDLE;
    UNLOCK_MUTEX(mode_mutex);
  }
  void space_bar_pressed(){
    if(halt_turret){
      return;
    }

  }
  uint32_t Turret::cooldown_between_shots(){
    return 1000;
  }
  bool Turret::can_fire_again() {
    return m_last_fire_tick + cooldown_between_shots() <= tick::get();
  }
  void Turret::move_to(const int32_t& x,const int32_t& y) {
    self.rect.x = x;
    self.rect.y = y;
  }
  void Turret::perform_ai() {
    if(sweep_forward){
      angle += 1;
      if(angle == start_angle + 90){
        sweep_forward = false;
      }
    }else{
      angle -= 1;
      if(angle == start_angle - 90){
        sweep_forward = true;
      }
    }
  }
  void tick() {
    if(halt_turret){
      return;
    }
    for(auto& s : turret_list) {
      if(!s->dispatched){
        continue;
      }
      s->tick();
      SDL_RenderCopyEx(
          ren,  //renderer
          s->self.bmp[0].texture,
          nullptr,// src rect
          &s->self.rect,
          s->angle, // angle
          nullptr,  // center
          SDL_FLIP_NONE // flip
          );
    }
  }
  SDL_Texture* Turret::initial_texture() {
    return self.bmp[0].texture;
  }
  void Turret::calc() {
    cx = self.rect.x + self.rect.w / 2;
    cy = self.rect.y + self.rect.h / 2;
    angle = 0; // FIXME
  }
  void Turret::tick() {
    perform_ai();
      SDL_RenderCopyEx(
          ren,  //renderer
          self.bmp[0].texture,
          nullptr,// src rect
          &self.rect,
          angle, // angle
          nullptr,  // center
          SDL_FLIP_NONE // flip
          );
  }
  Asset* Turret::next_state() {
    return states[0];
  }
  int Turret::gun_damage() {
    return rand_between(550,850);
  }

  Turret::Turret(const int32_t& _x,
      const int32_t& _y){
    call_count = 0;
    self.rect.x = _x;
    self.rect.y = _y;
    self.rect.w = TURRET_WIDTH;
    self.rect.h = TURRET_HEIGHT;
    movement_amount = 450;
    self.load_bmp_asset(BMP.data());

    hp = 550;
    max_hp = 850;
    ready = true;

    state_index = 0;
    calc();
    m_last_fire_tick = 0;
    move_to(_x,_y);
    dispatched = true;
    sweep_forward = true;

    switch(dir::get_facing(plr::get()->angle)) {
      case Direction::SOUTH_EAST:
        /** Facing SOUTH EAST */
        start_angle = 360 - 45;
        break;
      case Direction::EAST:
        /** FACING EAST */
        start_angle = 360 - 90;
        break;
      case Direction::NORTH_EAST:
        /** FACING NORTH EAST */
        start_angle = 270 + 45;
        break;
      case Direction::NORTH:
        /** FACING NORTH */
        start_angle = 270 - 90;
        break;
      case Direction::NORTH_WEST:
        /** FACING NORTH WEST */
        start_angle = 180 - 45;
        break;
      case Direction::WEST:
        /** FACING WEST */
        start_angle = 180 - 90;
        break;
      case Direction::SOUTH_WEST:
        /** FACING SOUTH WEST */
        start_angle = 90;
        break;
      default:
      case Direction::SOUTH:
        /** FACING SOUTH */
        start_angle = 0;
        break;
    }
    
    angle = start_angle;

  }
  void Turret::deploy_at(const SDL_Point& p){
    hp = 550;
    max_hp = 850;
    ready = true;

    state_index = 0;
    m_last_fire_tick = 0;
    move_to(p.x,p.y);
    calc();
    deployed_at = tick::get();
    deployment_location = p;
    dispatched = true;
  }
  void program_exit(){
    halt_turret = true;
    turret_list.clear();
  }
};

#undef m_debug
