#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "f35.hpp"
#include "../player.hpp"
#include "../direction.hpp"
#include "../npc/paths.hpp"
#include "../font.hpp"
#include "../colors.hpp"
#include "../rng.hpp"
#include "../sound/gunshot.hpp"
#include "../sound/npc.hpp"
#include "../viewport.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[AIR-SUPPORT][F35][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[AIR-SUPPORT][F35][ERROR]: " << A << "\n";

namespace air_support::f35 {
  static constexpr std::size_t F35_WIDTH = 375;
  static constexpr std::size_t F35_HEIGHT = 260;
  static constexpr uint16_t FACE_SOUTH = 90;
  static constexpr uint16_t FACE_EAST = 0;
  static constexpr uint16_t MOVEMENT_AMOUNT = 80;
  static constexpr int AIRCRAFT_CARRIER_X_POSITION = -20800;
  static std::vector<SDL_Point> bomb_targets;
  void move_map(int dir, int amount){
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
  }
  void draw_target(SDL_Point p){
    SDL_Rect r;
    r.x = p.x;
    r.y = p.y;
    r.w = 50;
    r.h = 50;
    draw::blatant_rect(&r);
  }

  int initial_x = 0;
  int initial_y = 0;
  static inline std::vector<SDL_Point> get_bomb_spots(){
    const int& x = initial_x;
    const int& y = initial_y;
    return std::vector<SDL_Point>{{x,y},
      {x+125,y-125},
      {x+400,y-125},
      {x+125,y+125},
      {x+400,y+125},
    };
  }
  static inline void cast_rays(){
    const int& x = initial_x;
    const int& y = initial_y;
    draw_target({x,y});
    draw_target({x+125,y-125});
    draw_target({x+400,y-125});
    draw_target({x+125,y+125});
    draw_target({x+400,y+125});
  }
  void F35::calculate_aim() {
    target_x = plr::get_cx();
    target_y = plr::get_cy();
  }
  void spawn(const int& in_start_x, const int& in_start_y) {
    f35_list.emplace_front(in_start_x,in_start_y);
  }
  void position_fleet_at_aircraft_carrier(){
    int start_x = AIRCRAFT_CARRIER_X_POSITION;
    int i=1;
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
    m_debug("F35 squadron returned to aircraft carrier");
  }
  std::vector<SDL_Point> bomb_impacts;
  uint32_t dispatch_bombs_at(uint32_t interval, void* in_vec_list_ptr){
    return 5000;
  }
  uint32_t dispatch_func(uint32_t interval, void* name){
    std::cout << "dispatch_now\n";
    dispatch_now();
    SDL_AddTimer(5000,dispatch_bombs_at,const_cast<char*>("F35"));
    return 20000;
  }
  void init() {
    int start_x = AIRCRAFT_CARRIER_X_POSITION;
    for(int i=1; i <= 3; ++i){
      spawn(start_x + (-2080 * i), (i - 1) * F35_HEIGHT);
    }
    for(int i=1; i <= 3; ++i){
      spawn(start_x + (-3080 * i), (i - 1) * F35_HEIGHT);
    }
    SDL_TimerID id = SDL_AddTimer(5000,dispatch_func,const_cast<char*>("F35"));
    std::cout << "time id: " << id << "\n";
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
    static bool initial_set = false;
    if(!initial_set){
      std::cout << "initial_set is false\n";
      initial_x = plr::cx();
      initial_y = plr::cy();
      initial_set = true;
      bomb_targets = get_bomb_spots();
    }
    save_draw_color();
    set_draw_color("red");
    for(const auto& point : bomb_targets){
      draw_target(point);
    }
    restore_draw_color();

    //auto rays = cast_rays();
    //SDL_RenderDrawPoints(ren,&rays[0],rays.size());
    //for(const auto& impact : bomb_impacts){
    //    SDL_Rect r;
    //    r.x = impact.x;
    //    r.y = impact.y;
    //    r.w = 250;
    //    r.h = 250;
    //    draw::red_letter_at(&r,"LOL",250);
    //    draw::line(0, 0, impact.x,impact.y);
    //}
    for(auto& s : f35_list) {
      if(!s.dispatched()){
        continue;
      }
      draw::red_letter_at(plr::get_rect(),"AIR STRIKE INBOUD",120);
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
  std::size_t dispatch_now(){
    std::size_t dispatched_jets = 0;
    std::size_t i = 0;
    for(auto& jet : f35_list){
      jet.call_in_airstrike(i);
      jet.m_dispatched = true;
      jet.on_carrier = false;
      jet.randomize_bombing_targets();
      ++dispatched_jets;
      ++i;
    }
    return dispatched_jets; 
  }
  void F35::randomize_bombing_targets(){
    bombing_targets.clear();
    for(size_t i=0; i < 8; i++){
      SDL_Point p;
      p.x = initial_x + 50;
      p.y = initial_y;
      draw::line(self.rect.x, self.rect.y,p.x,p.y);
      bombing_targets.emplace_back(p);
    }
  }
  void return_to_carrier(){
    position_fleet_at_aircraft_carrier();
    for(auto& jet : f35_list){
      jet.m_dispatched = false;
      jet.on_carrier = true;
    }
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
};

#undef m_debug
