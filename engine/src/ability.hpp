#ifndef __ABILITY_HEADER__
#define __ABILITY_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <array>

#include "asset.hpp"

namespace ability {
  struct Ability {
    std::string name;
    bool unlocked;
  };

  void init();
  void tick();
  void program_exit();
};
#endif
