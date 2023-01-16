#ifndef __NPC_SPETSNAZ_HEADER__
#define __NPC_SPETSNAZ_HEADER__
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include "debug.hpp"
#include "extern.hpp"
#include "behaviour-tree.hpp"

namespace npc {
  static constexpr std::size_t SPETSNAZ_MAX = 1;
  static constexpr std::size_t SPETS_WIDTH = 80;
  static constexpr std::size_t SPETS_HEIGHT = 53;
  static constexpr std::size_t SPETS_MOVEMENT = 2;
  static constexpr std::size_t BULLET_POOL_SIZE = 1024;
  static constexpr const char* BMP = "../assets/spet-0.bmp";
  static constexpr const char* HURT_BMP = "../assets/spet-hurt-%d.bmp";
  static constexpr std::size_t HURT_BMP_COUNT = 3;
  static constexpr const char* DEAD_BMP = "../assets/spet-dead-%d.bmp";
  static constexpr std::size_t DEAD_BMP_COUNT = 1;

  static constexpr int SPETSNAZ_MAX_HP = 100;
  static constexpr int SPETSNAZ_LOW_HP = 75;
  static constexpr int SPETSNAZ_RANDOM_LO = 10;
  static constexpr int SPETSNAZ_RANDOM_HI = 25;

  std::vector<Actor*> dead_list;
  struct Spetsnaz {
    struct Hurt {
      Actor self;
    };
    struct Dead {
      Actor self;
    };

    Actor self;
    int movement_amount;
    int cx;
    int cy;
    SDL_Rect& dest = self.rect;
    Hurt hurt_actor;
    Dead dead_actor;
    int hp;
    int max_hp;
    int angle;
    bool ready;
    std::vector<Asset*> states;
    std::size_t state_index;
    const bool is_dead() const {
      return hp <= 0;
    }

    void init_with(const int32_t& _x,
        const int32_t& _y,
        const int& _ma) {
      self.rect.x = _x;
      self.rect.y = _y;
      self.rect.w = SPETS_WIDTH;
      self.rect.h = SPETS_HEIGHT;
      movement_amount = _ma;
      self.load_bmp_asset(BMP);

      hurt_actor.self.load_bmp_assets(HURT_BMP,HURT_BMP_COUNT);
      dead_actor.self.load_bmp_assets(DEAD_BMP,DEAD_BMP_COUNT);
      hp = rand_between(SPETSNAZ_LOW_HP,SPETSNAZ_MAX_HP);
      max_hp = hp + rand_between(SPETSNAZ_RANDOM_LO,SPETSNAZ_RANDOM_HI);
      ready = true;

      state_index = 0;
      for(int i=0; i < hurt_actor.self.bmp.size();++i){
        states.emplace_back(&hurt_actor.self.bmp[i]);
      }
      calc();
    }
    Spetsnaz() : ready(false) {}
    /** Copy constructor */
    Spetsnaz(const Spetsnaz& other) = delete;

    SDL_Texture* initial_texture(){
      return self.bmp[0].texture;
    }
    void calc(){
      plr::calc();
      angle = coord::get_angle<Spetsnaz>(*this,plr::get_cx(),plr::get_cy());
      cx = self.rect.x + self.rect.w / 2;
      cy = self.rect.y + self.rect.h / 2;
    }
    void tick(){
      if(is_dead()){
        return;
      }
      //dest.x += movement_amount;
      //dest.y = self.rect.y;
      calc();
      perform_ai();
    }
    Asset* next_state(){
      if(state_index == states.size()){
        return &dead_actor.self.bmp[0];
      }
      return states[state_index++];
    }

    void take_damage(int damage){
      hp -= damage;
      if(hp <= 0){
        self.bmp[0] = dead_actor.self.bmp[rand_between(0,dead_actor.self.bmp.size()-1)];
        dead_list.emplace_back(&self);
        return;
      }
      self.bmp[0] = *next_state();
    }
    void perform_ai();
    void move_left(){
      self.rect.x -= movement_amount;
    }
    void move_right(){
      self.rect.x += movement_amount;
    }
  };


  static std::array<Spetsnaz,SPETSNAZ_MAX> spetsnaz_list;

  void init_spetsnaz(){
    for(size_t i=0; i < SPETSNAZ_MAX; ++i){
      spetsnaz_list[i].init_with(win_width() / 10, win_height() / 10,SPETS_MOVEMENT);
      world->npcs.push_front(&spetsnaz_list[i].self);
    }
  }
  void Spetsnaz::perform_ai(){
    if(plr::get_cx() < cx){
      move_left();
    }
    if(plr::get_cx() > cx){
      move_right();
    }
    if(plr::get_cx() == cx){
      std::cout << "FIRE\n";
    }
  }
  void spetsnaz_tick(){
    for(auto& s : spetsnaz_list){
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
  void spetsnaz_movement(int adjustment){
    for(auto& s : spetsnaz_list){
      if(s.is_dead()){
        continue;
      }
      s.self.rect.x += adjustment;
    }
  }
  void take_damage(Actor* a,int dmg){
    for(auto& s : spetsnaz_list){
      if(&s.self == a){
        s.take_damage(dmg);
      }
    }
  }
  bool is_dead(Actor* a){
    return std::find( dead_list.cbegin(), dead_list.cend(), a) != dead_list.cend();
  }
};
#endif
