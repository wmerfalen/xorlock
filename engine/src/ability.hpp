#ifndef __ABILITY_HEADER__
#define __ABILITY_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <array>

#include "asset.hpp"

enum ability_t : uint32_t {
  F35_AIR_SUPPORT,
  TURRET,
  AERIAL_DRONE,
};
namespace ability {
  struct Ability {
    ability_t id;
    std::string name;
    bool unlocked;
    size_t charges;
    Ability(ability_t _id,size_t _ch) : id(_id), charges(_ch) {
      name = to_string();
      unlocked = true; // TODO: change depending on loadout/class
    }
    std::string to_string() {
      switch(id){
        case F35_AIR_SUPPORT: return "f35";
        case TURRET: return "turret";
        case AERIAL_DRONE: return "drone";
        default: return "?";
      }
    }
  };

  void init();
  void tick();
  void program_exit();
};
#endif
