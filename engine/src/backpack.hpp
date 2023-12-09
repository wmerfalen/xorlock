#ifndef __BACKPACK_HEADER__
#define __BACKPACK_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include <variant>
#include "actor.hpp"
#include "world.hpp"
#include "direction.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include <map>

#include "draw.hpp"
#include "draw-state/player.hpp"
#include "draw-state/reticle.hpp"
#include <memory>
#include "npc-id.hpp"
#include "constants.hpp"
#include "weapons.hpp"
#include "events/death.hpp"

namespace backpack {
  using ExportWeapon = events::death::ExportWeapon;
  using ExportGrenade = events::death::ExportGrenade;

  struct Backpack {
    void load();
    std::forward_list<std::unique_ptr<ExportWeapon>> weapons;
    std::forward_list<std::unique_ptr<ExportGrenade>> grenades;
    Backpack();
    Backpack(const Backpack& other) = delete;
  };
  void init();
  void tick();
  void program_exit();
};

#endif
