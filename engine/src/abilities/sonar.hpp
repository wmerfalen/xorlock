#ifndef __ABILITIES_SONAR_HEADER__
#define __ABILITIES_SONAR_HEADER__
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include "../actor.hpp"
#include "../world.hpp"
#include "../coordinates.hpp"
#include "../bullet-pool.hpp"
#include "../debug.hpp"
#include "../draw.hpp"

#include "../behaviour-tree.hpp"
#include "../npc-id.hpp"
#include "../npc/paths.hpp"
#include "../weapons.hpp"
#include "../constants.hpp"
#include "../weapons/primary.hpp"
#include "../player.hpp"

namespace abilities::sonar {
  void move_map(int direction,int amount);
  void init();
  void tick();
  void space_bar_pressed();
  void program_exit();
};
#endif
