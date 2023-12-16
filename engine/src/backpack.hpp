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
#include "loot.hpp"

namespace backpack {
  using ExportWeapon = loot::ExportWeapon;
  using ExportGrenade = loot::ExportGrenade;
  using type_t = loot::type_t;
  using loot_id_t = loot::loot_id_t;

  struct Backpack {
    void load();
    void save();
    std::forward_list<std::unique_ptr<ExportWeapon>> weapons;
    std::forward_list<std::unique_ptr<ExportGrenade>> grenades;
    std::vector<ExportWeapon*> weapons_ptr;
    std::vector<ExportGrenade*> grenades_ptr;
    Backpack();
    Backpack(const Backpack& other) = delete;
    std::tuple<bool,std::string> put_item(const loot_id_t & id,type_t type);
    void remove_item(const loot_id_t& id);
    void refresh();
    std::pair<bool,std::string> wield_primary(ExportWeapon* ptr);
    std::pair<bool,std::string> wield_secondary(ExportWeapon* ptr);
    std::pair<bool,std::string> wield_frag(ExportGrenade* ptr);
    ExportWeapon* get_primary();
    ExportWeapon* get_secondary();
    ExportGrenade* get_frag();
    wpn::weapon_t get_weapon_type(ExportWeapon* ptr);
    wpn::weapon_t get_weapon_type(ExportGrenade* ptr);
  };
  void init();
  void tick();
  void program_exit();
};

#endif
