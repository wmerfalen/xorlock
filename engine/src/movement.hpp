#ifndef __MOVEMENT_HEADER__
#define __MOVEMENT_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "actor.hpp"
#include "player.hpp"
#include "world.hpp"

struct MovementManager {
  MovementManager() = default;
  ~MovementManager() = default;
  void wants_to_move(const World& world,
      Actor& actor,
      Direction dir) {

  }
};

#endif
