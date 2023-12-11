#ifndef __EVENTS_DEATH_HEADER__
#define __EVENTS_DEATH_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include <variant>
#include "../loot.hpp"

namespace events::death {
  void dispatch(
      constants::npc_type_t npc_type,
      npc_id_t id,
      int in_cx,
      int in_cy);
  //void pickup_loot(const Loot* loot_ptr);

  void init();

  void tick();
  void program_exit();
  void move_map(int,int);
};

#endif
