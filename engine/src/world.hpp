#ifndef __WORLD_HEADER__
#define __WORLD_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "actor.hpp"

struct World {
  uint32_t width;
  uint32_t height;

  std::vector<Actor> npcs;
};


#endif
