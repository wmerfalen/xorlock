#ifndef __CONSTANTS_HEADER__
#define __CONSTANTS_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <memory>
#include <iomanip>
#include "actor.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "viewport.hpp"
#include "circle.hpp"
#include <vector>
#include <deque>
#include "clock.hpp"
#include "rng.hpp"
#include "draw.hpp"
#include "line.hpp"
#include "npc-id.hpp"


namespace constants {
  enum npc_type_t {
    NPC_SPETSNAZ,
  };
	static constexpr int	BULLET_TRAIL_HEIGHT =  20;
	static constexpr int BULLET_TRAIL_WIDTH = 5;
  static constexpr const char* gunshot_dir = "../assets/sound/gunshot/";
  static constexpr const char* npc_pain_dir = "../assets/sound/npc/pain/";
  static constexpr const char* npc_death_dir = "../assets/sound/npc/death/";
  static constexpr const char* npc_corpse_dir = "../assets/sound/npc/death/corpse/";
  static constexpr const char* reload_dir = "../assets/sound/reload/";
  static constexpr const char* music_dir = "../assets/sound/tracks/";
  static constexpr const char* mp5_gunshot_wave = "p4";
  static constexpr const char* mp5_reload_wave = "a0";
  static constexpr const char* music_list[] = {
    "track-01-camo",
    "track-02-ghost",
    nullptr,
  };
};

#endif
