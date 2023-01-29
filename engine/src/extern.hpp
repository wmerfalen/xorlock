#ifndef __EXTERN_HEADER__
#define __EXTERN_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>
#include <array>

#include <functional>
#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";

extern void travel_to(const int& x,const int& y);
namespace timeline {
	using callback_t = std::function<void(void*)>;
	enum interval_t : uint16_t {
		MS_10 = 10,
		MS_50 = 50,
		MS_100 = 100,
		MS_250 = 250,
		MS_500 = 500,
		SEC_1 = 1000,
	};
	extern void register_timeline_event(
	    int count,
	    interval_t n,
	    timeline::callback_t f);
};
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
namespace rng {
	extern void init();
	extern bool chaos();
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

namespace colors {
	uint8_t g[] = {0,255,0};
	static inline uint8_t* green() {
		return g;
	}
	uint8_t _bullet_line[] = {255, 255, 204};
	static inline uint8_t* bullet_line() {
		return _bullet_line;
	}
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
	extern int cx();
	extern int cy();
	extern void calc();
	extern void start_gun();
	extern void stop_gun();
	extern bool should_fire();
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
