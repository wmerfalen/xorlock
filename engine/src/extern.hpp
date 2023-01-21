#ifndef __EXTERN_HEADER__
#define __EXTERN_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>
#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";

extern void travel_to(const int& x,const int& y);

#include "line.hpp"
namespace npc {
	extern void spetsnaz_movement(int);
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
