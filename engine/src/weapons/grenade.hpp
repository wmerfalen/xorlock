#ifndef __WEAPONS_GRENADE_HEADER__
#define __WEAPONS_GRENADE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "../weapons.hpp"
#include "../actor.hpp"
#include "../tick.hpp"
#include "../rng.hpp"
#include "../timeline.hpp"
#include "../line.hpp"

namespace weapons {
  struct Grenade {
    explosive_stats_t* stats;
    Grenade(explosive_stats_t* in_stats);
    Grenade(const SDL_Point& src,const SDL_Point& dst);
    Grenade();
    Grenade(const Grenade& other) = delete;
    void set_grenade(explosive_stats_t* in_stats,const int32_t& src_x,const int32_t& src_y);
    int hold_grenade();
    int toss_towards(const int32_t& dst_x, const int32_t& dst_y);
    void explode();
    void tick();
    bool done();
    SDL_Point source;
    SDL_Point dest;
    SDL_Rect dest_rect;
    Actor self;
    bool m_done;
		Line line;
    std::size_t line_index;
    SDL_Rect result;
  };
  namespace grenade {
    void init();
    void tick();
    void register_traveler(Grenade* ptr);
    void move_map(int dir,int amount);
  };
};
#endif
