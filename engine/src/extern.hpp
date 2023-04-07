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
namespace rng {
	extern void init();
	extern bool chaos();
};
//namespace cursor {
//	extern int mx();
//	extern int my();
//};
//#include "line.hpp"
//namespace npc {
//	extern void spetsnaz_movement(uint8_t,int);
//};
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


//extern void move_map_by(int,int amount);


#endif
