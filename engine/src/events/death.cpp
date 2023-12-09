#include "death.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[EVENTS][DEATH][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[EVENTS][DEATH][ERROR]: " << A << "\n";

namespace events::death {
  static std::vector<SDL_Point> graves;
  static SDL_mutex* graves_mutex = SDL_CreateMutex();
  void init(){
    m_debug("death init");
  }

  void tick(){

  }
  void program_exit(){

  }
  void move_map(int dir, int amount){
    LOCK_MUTEX(graves_mutex);
    for(auto& exp: graves){
      switch(dir) {
        case NORTH_EAST:
          exp.y += amount;
          exp.x -= amount;
          break;
        case NORTH_WEST:
          exp.y += amount;
          exp.x += amount;
          break;
        case NORTH:
          exp.y += amount;
          break;
        case SOUTH_EAST:
          exp.y -= amount;
          exp.x -= amount;
          break;
        case SOUTH_WEST:
          exp.y -= amount;
          exp.x += amount;
          break;
        case SOUTH:
          exp.y -= amount;
          break;
        case WEST:
          exp.x += amount;
          break;
        case EAST:
          exp.x -= amount;
          break;
        default:
          break;
      }
    }
    UNLOCK_MUTEX(graves_mutex);
  }

  void dispatch(constants::npc_type_t npc_type, npc_id_t id, int in_cx, int in_cy){
    m_debug("npc died");
  }
};
