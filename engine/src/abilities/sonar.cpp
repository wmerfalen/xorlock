#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "sonar.hpp"
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
#include "../sound/gunshot.hpp"
#include "../npc-bomber.hpp"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_mutex.h>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[SONAR][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SONAR][ERROR]: " << A << "\n";

namespace abilities::sonar {
  bool halt_sonar = false;
  size_t color_index = 0;
  uint8_t opacity;
  bool start;
  static constexpr uint8_t TICK_MOD = 5;
  static constexpr uint8_t OPACITY_DECREMENT = 5;
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
  //SDL_mutex sonar_list_mutex = SDL_CreateMutex();
  void init(){
    halt_sonar = false;
    opacity = 255;
    start = false;
  }
  void move_map(int dir, int amount){
    if(halt_sonar){
      return;
    }
  }
  void draw(int x, int y, int tox, int toy){
		static const auto color = colors::green();
		save_draw_color();
    SDL_SetRenderDrawBlendMode(ren,SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(ren,colors[color_index][0],colors[color_index][1],colors[color_index][2],opacity);
		SDL_RenderDrawLine(ren,
		                   x,
		                   y,
		                   tox,
		                   toy
		                  );
		restore_draw_color();
  }
  void space_bar_pressed(){
    opacity = 255;
    start = true;
  }

  void tick() {
    if(halt_sonar || !start){
      return;
    }
    if((tick::get() % TICK_MOD) == 0){
      int test = opacity - OPACITY_DECREMENT;
      if(test <= 0){
        start = false;
        return;
      }
      opacity -= OPACITY_DECREMENT;
    }
    
    for(const auto& mob : npc::bomber::data::bomber_list){
      if(mob.hp > 0){
        draw(plr::cx(),plr::cy(),mob.cx,mob.cy);
      }
    }
    for(const auto& mob : npc::spetsnaz_list){
      if(mob.hp > 0){
        draw(plr::cx(),plr::cy(),mob.cx,mob.cy);
      }
    }
  }

  void program_exit(){
    halt_sonar = true;
  }
};

#undef m_debug
