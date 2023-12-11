#include "death.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "../filesystem.hpp"
#include "../weapons/pistol/p226.hpp"
#include "../weapons.hpp"
#include "../player.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[EVENTS][DEATH][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[EVENTS][DEATH][ERROR]: " << A << "\n";

namespace events::death {
  void init(){
  }

  void tick(){
  }
  void program_exit(){
  }
  void dispatch(constants::npc_type_t npc_type, npc_id_t id, int in_cx, int in_cy){
    m_debug("npc died");
    loot::dispatch(npc_type,id,in_cx,in_cy);
  }
};
