#ifndef __WEAPONS_GRENADE_HEADER__
#define __WEAPONS_GRENADE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "../weapons.hpp"
#include "../tick.hpp"
#include "../rng.hpp"
#include "../timeline.hpp"
#include "../line.hpp"

namespace weapons {
  namespace grenade {
    void init();
    void tick();
  };
  struct Grenade {
    explosive_stats_t* stats;
    Grenade(explosive_stats_t* in_stats);
    Grenade(const SDL_Point& src,const SDL_Point& dst);
    Grenade() = delete;
    Grenade(const Grenade& other) = delete;
    int hold_grenade();
    int toss_towards(const int32_t& dst_x, const int32_t& dst_y);
    void explode();
    void tick();
    bool done();
    SDL_Point source;
    SDL_Point dest;
    bool m_done;
		Line line;
    std::size_t line_index;
  };
};
#endif
