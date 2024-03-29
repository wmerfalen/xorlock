#ifndef __LOOT_HEADER__
#define __LOOT_HEADER__
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

namespace loot {
  using loot_id_t = uint64_t;
  enum type_t : uint32_t {
    GUN,
    EXPLOSIVE,
  };
  struct ExportWeapon {
    type_t object_type;
    loot_id_t id;
    wpn::weapon_type_t type;
    weapon_stats_t stats;
    std::array<char,64> name;
  };
  struct ExportGrenade {
    type_t object_type;
    loot_id_t id;
    wpn::grenade_type_t type;
    explosive_stats_t stats;
    std::array<char,64> name;
  };
  struct Loot {
    Loot() = delete;
    ~Loot();
    Loot(const Loot&) = delete;
    Loot(int npc_type,int npc_id,int cx, int cy);
    void handle_bomber(const int& npc_id);
    void handle_spetsnaz(const int& npc_id);
    type_t object_type;
    loot_id_t id;
    std::string name;
    type_t type;
    int item_type;
    std::variant<weapon_stats_t,explosive_stats_t> stats;
    Actor self;
    void dump();
    void write_to_file();
    bool is_gun() const;
    ExportWeapon export_weapon();
    ExportGrenade export_grenade();
  };
  std::vector<Loot*> near_loot(SDL_Rect* r);
  void dispatch(
      constants::npc_type_t npc_type,
      npc_id_t id,
      int in_cx,
      int in_cy);
  void pickup_loot(const Loot* loot_ptr);

  void init();

  void tick();
  void program_exit();
};

#endif
