#ifndef __DB_HEADER__
#define __DB_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "actor.hpp"
#include <map>
#include <memory>

namespace db {
  void init();
  uint64_t next_id();
};

#endif
