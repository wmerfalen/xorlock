#ifndef __EXTERN_HEADER__
#define __EXTERN_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>
#include <array>

#include <functional>
#include "npc-id.hpp"
#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";

//extern void travel_to(const int& x,const int& y);
namespace timeline {
	using callback_t = std::function<void(void*)>;
	enum interval_t : uint16_t {
		MS_2 = 2,
		MS_5 = 5,
		MS_10 = 10,
		MS_15 = 15,
		MS_20 = 20,
		MS_25 = 25,
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
	WPN_DMG_LO = 1,
	WPN_DMG_HI = 2,
	WPN_BURST_DLY = 3,
	WPN_PIXELS_PT = 4,
	WPN_CLIP_SZ = 5,
	WPN_AMMO_MX = 6,
	WPN_RELOAD_TM = 7,
	WPN_COOLDOWN_BETWEEN_SHOTS = 8,
	WPN_MS_REGISTRATION = 9,
};
using weapon_stats_t = std::array<uint32_t,10>;

//namespace cursor {
//	extern int mx();
//	extern int my();
//};
//#include "line.hpp"
namespace npc {
	extern void spetsnaz_movement(uint8_t,int);
};
//namespace cd {
//	extern std::vector<void*> npcs_hit_by_bullet(const Line&);
//};
//namespace bullet {
//	extern void queue_npc_bullets(const npc_id_t& id,weapon_stats_t* stats_ptr,int in_cx, int in_cy,int dest_x, int dest_y);
//};

namespace colors {
	static uint8_t g[] = {0,255,0};
	static uint8_t _red[] = {255,0,0};
	static uint8_t _blue[] = {0,0,255};
	static inline uint8_t* green() {
		return g;
	}
	static inline uint8_t* red() {
		return _red;
	}
	static inline uint8_t* blue() {
		return _blue;
	}
	static uint8_t _bullet_line[] = {255, 255, 204};
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
namespace saved {
	static uint8_t r,g,b,a;
};

static uint8_t GREEN[] = {0,255,0};


extern void move_map_by(int,int amount);
static inline int rand_between(const int& min,const int& max) {
	return rand()%(max-min + 1) + min;
}
static inline int rand_between(weapon_stats_t* stats) {
	return rand_between((*stats)[WPN_DMG_LO],(*stats)[WPN_DMG_HI]);
}


#endif
