#ifndef __EVENTS_DEATH_HEADER__
#define __EVENTS_DEATH_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "../actor.hpp"
#include "../world.hpp"
#include "../direction.hpp"
#include "../triangle.hpp"
#include "../coordinates.hpp"
#include "../bullet-pool.hpp"
#include <map>

#include "../draw.hpp"
#include "../draw-state/player.hpp"
#include "../draw-state/reticle.hpp"
#include <memory>
#include "../npc-id.hpp"
#include "../constants.hpp"

namespace events::death {
  void dispatch(
      constants::npc_type_t npc_type,
      npc_id_t id,
      int in_cx,
      int in_cy);

  void init();

  void tick();
  void program_exit();
  void move_map(int,int);
};

#endif
