#ifndef __EVENTS_DEATH_HEADER__
#define __EVENTS_DEATH_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include <variant>
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
#include "../weapons.hpp"

namespace events::death {
  struct ExportWeapon {
    uint64_t id;
    wpn::weapon_type_t type;
    weapon_stats_t stats;
  };
  struct ExportGrenade {
    uint64_t id;
    wpn::grenade_type_t type;
    explosive_stats_t stats;
  };
  struct Loot {
    enum type_t {
      GUN,
      EXPLOSIVE,
    };
    Loot() = delete;
    ~Loot() = default;
    Loot(const Loot&) = delete;
    Loot(int npc_type,int npc_id,int cx, int cy);
    uint64_t id;
    type_t type;
    int item_type;
    std::variant<weapon_stats_t,explosive_stats_t> stats;
    SDL_Point where;
    void dump();
    void write_to_file();
    ExportWeapon export_weapon();
    ExportGrenade export_grenade();
  };
  std::vector<Loot*> near_loot(SDL_Rect* r);
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
