#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "drone.hpp"
#include "../player.hpp"
#include "../actor.hpp"
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
#include "../sound/gunshot.hpp"
#include "../npc-bomber.hpp"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_mutex.h>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[SONAR][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SONAR][ERROR]: " << A << "\n";

namespace abilities::drone {
  bool halt_drone = false;
  bool has_active_drone = false;
  size_t color_index = 0;
  bool start;
  static constexpr uint8_t TICK_MOD = 5;
  static constexpr uint8_t OPACITY_DECREMENT = 2;
  static constexpr uint8_t DRONE_AUDIO_CHANNEL = 4;
  static constexpr uint8_t DRONE_AUDIO_CHANNEL_2 = 5;
  std::forward_list<std::unique_ptr<Drone>> drone_list;
  SDL_mutex* drone_list_mutex = SDL_CreateMutex();
  Mix_Chunk* drone_zero_wave = nullptr;
  std::array<Mix_Chunk*,2> drone_loiter_waves;
  std::array<std::array<uint8_t,3>,4> colors = {
    std::array<uint8_t,3>{
      255, // red
      0,    // green
      0,    // blue
    },
    std::array<uint8_t,3>{
      0, // red
      255,    // green
      0,    // blue
    },
    std::array<uint8_t,3>{
      0, // red
      0,    // green
      255,    // blue
    },
    std::array<uint8_t,3>{
      10, // red
      10,    // green
      10,    // blue
    },
  };
  Drone::Drone() : x(plr::cx()),y(plr::cy()),cx(x),cy(y) {
    charge = 3; // TODO: dynamically allocate this
    call_counter = 0;
    state = BOOT_UP;
    target_angle = 0;
    angle = 0;
    hp = 250;
    max_hp = 250;
    ready = true;
    self.rect.x = x;
    self.rect.y = y;
    self.rect.w = 10;
    self.rect.h = 10;
    movement_amount = 50;
    static constexpr const char* BMP = "../assets/drone.bmp";
    self.load_bmp_asset(BMP);
    register_actor(&self);
    state = BOOT_UP;
    move_at = 0;
    opacity = 255;
    done_at = tick::get() + 20000;
    Mix_PlayChannel(DRONE_AUDIO_CHANNEL,drone_zero_wave,0);
    loiter_wave = -1;
  }
  Drone::~Drone(){
    unregister_actor(&self);
  }
  void Drone::tick(){
    if(done_at < tick::get()){
      state = SELF_DESTRUCT;
    }
    if(state == SELF_DESTRUCT){
      return;
    }
    ++call_counter;
    if(state == BOOT_UP){
      self.rect.w = 10;
      self.rect.h = 10;
      if(call_counter == 20){
        state = ASCEND_START;
        call_counter = 0;
      }
      has_active_drone = true;
    }
    if(state == ASCEND_START){
      if(call_counter < 90){
        self.rect.x += 5;
        self.rect.w = 10 + (call_counter * 4.30);
        self.rect.h = 10 + (call_counter * 4.30);
        if(call_counter > 15 && call_counter < 85){
          angle += 3;
        }
      }else{
        self.rect.x += 50;
      }
      has_active_drone = true;
    }
    if(state == ASCEND_START && call_counter > 110){
      state = LOITER;
      ++loiter_wave;
      Mix_PlayChannel(DRONE_AUDIO_CHANNEL,drone_loiter_waves[0],0);
    }
    if(state == LOITER && call_counter > 250 && loiter_wave == 0){
      Mix_PlayChannel(DRONE_AUDIO_CHANNEL_2,drone_loiter_waves[0],0);
      ++loiter_wave;
    }
    if(state == LOITER && call_counter > 330 && loiter_wave == 1){
      Mix_PlayChannel(DRONE_AUDIO_CHANNEL,drone_loiter_waves[1],0);
      ++loiter_wave;
    }
    if(state == LOITER){
      if((tick::get() % TICK_MOD) == 0){
        int test = opacity - OPACITY_DECREMENT;
        if(test <= 0){
          if(charge - 1 < 0){
            charge = 0;
          }else{
            --charge;
          }
          if(charge){
            opacity = 255;
          }
        }else{
          opacity -= OPACITY_DECREMENT;
        }
      }

      self.rect.w = 80;
      self.rect.h = 80;
      if(move_at < tick::get()){
        if(self.rect.x > plr::cx() + 180){
          self.rect.x -= 10;
        }
        if(self.rect.y > plr::cy() - 380){
          self.rect.y -= 10;
        }
        if(self.rect.x < plr::cx() + 180){
          self.rect.x += 10;
        }
        if(self.rect.y < plr::cy() - 380){
          self.rect.y += 10;
        }
        if(self.rect.x > plr::cx() + 150 && self.rect.x < plr::cx() + 200
            && self.rect.y > plr::cy() - 400 && self.rect.y < plr::cy() + 350){
          target_angle = 45;
        }
        if(target_angle > angle){
          ++angle;
        }else if(target_angle < angle){
          --angle;
        }
        move_at = tick::get() + 20;
      }
    }

    SDL_RenderCopyEx(
        ren,  //renderer
        self.bmp[0].texture,
        nullptr,// src rect
        &self.rect,
        angle, // angle
        nullptr,  // center
        SDL_FLIP_NONE // flip
        );
    calc();
    if(opacity == 0 && charge == 0){
      state = SELF_DESTRUCT;
    }
  }
  bool Drone::draw_lines(){
    return charge > 0;
  }
  void Drone::calc(){
    x = self.rect.x;
    y = self.rect.y;
    cx = x + self.rect.w / 2;
    cy = y + self.rect.h / 2;
  }
  //SDL_mutex drone_list_mutex = SDL_CreateMutex();
  void init(){
    drone_zero_wave = Mix_LoadWAV("../assets/sound/drone-0.wav");
    drone_loiter_waves[0] = Mix_LoadWAV("../assets/sound/drone-loiter-0.wav");
    drone_loiter_waves[1] = Mix_LoadWAV("../assets/sound/drone-loiter-1.wav");
    halt_drone = false;
    start = false;
    m_debug("drone::init()");
  }
  void draw(int x, int y, int tox, int toy,uint8_t opacity){
    SDL_SetRenderDrawColor(ren,colors[color_index][0],colors[color_index][1],colors[color_index][2],opacity);
    SDL_RenderDrawLine(ren,
        x,
        y,
        tox,
        toy
        );
  }
  void space_bar_pressed(){
    start = true;
    LOCK_MUTEX(drone_list_mutex);
    drone_list.emplace_front(std::make_unique<Drone>());
    UNLOCK_MUTEX(drone_list_mutex);
  }

  void tick() {
    if(halt_drone || !start){
      return;
    }
    LOCK_MUTEX(drone_list_mutex);
    save_draw_color();
    SDL_SetRenderDrawBlendMode(ren,SDL_BLENDMODE_BLEND);
    drone_list.remove_if([&](const auto& d) -> bool {
        return d->done_at <= tick::get();
    });
    for(auto& drone : drone_list){
      if(drone->draw_lines()){
        for(const auto& mob : npc::bomber::data::bomber_list){
          if(mob.hp > 0){
            draw(drone->cx,drone->cy,mob.cx,mob.cy,drone->opacity);
          }
        }
        for(const auto& mob : npc::spetsnaz_list){
          if(mob.hp > 0){
            draw(drone->cx,drone->cy,mob.cx,mob.cy,drone->opacity);
          }
        }
      }
      drone->tick();
    }
    restore_draw_color();
    SDL_SetRenderDrawBlendMode(ren,SDL_BLENDMODE_NONE);
    UNLOCK_MUTEX(drone_list_mutex);
  }

  void program_exit(){
    halt_drone = true;
  }
};

#undef m_debug
