#ifndef __EXTERN_HEADER__
#define __EXTERN_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>
#include <array>
#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";

extern void travel_to(const int& x,const int& y);
namespace wpn {
	enum Flags : uint32_t {
		SEMI_AUTOMATIC = (1 << 0),
		BURST_FIRE = (1 << 1),
		AUTOMATIC = (1 << 2),
		SEMI_AUTOMATIC_SHOTGUN = (1 << 3),
		AUTOMATIC_SHOTGUN = (1 << 4),
		PROJECTILE_LAUNCHER = (1 << 5),
	};
};
enum WPN {
	WPN_FLAGS = 0,
	WPN_DMG_LO,
	WPN_DMG_HI,
	WPN_BURST_DLY,
	WPN_PIXELS_PT,
	WPN_CLIP_SZ,
	WPN_AMMO_MX,
	WPN_COOLDOWN,
	WPN_RELOAD_TM,
};
using weapon_stats_t = std::array<uint32_t,8>;

namespace cursor {
	extern int mx();
	extern int my();
};
#include "line.hpp"
namespace npc {
	extern void spetsnaz_movement(uint8_t,int);
};
namespace cd {
	extern std::vector<void*> npcs_hit_by_bullet(const Line&);
};
extern int win_width();
extern int win_height();
extern int tile_width();
extern void ren_clear();
extern void ren_present();
extern void save_draw_color();
extern void set_draw_color(const char*);
extern void restore_draw_color();
extern SDL_Renderer* ren;
namespace plr {
	extern int get_cx();
	extern int get_cy();
	extern void calc();
};
namespace saved {
	uint8_t r,g,b,a;
};

uint8_t GREEN[] = {0,255,0};


extern void move_map_by(int,int amount);
extern floatPoint ms_point ;
extern floatPoint plr_point ;
extern floatPoint top_right;
extern floatPoint bot_right;
static inline int rand_between(const int& min,const int& max) {
	return rand()%(max-min + 1) + min;
}

namespace static_guy {
	extern void init();
};


#endif
