#ifndef __COLOR_HEADER__
#define __COLOR_HEADER__
#include <iostream>
#include <SDL2/SDL.h>

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
void save_draw_color();
void set_draw_color(const char*);
void restore_draw_color();
extern SDL_Renderer* ren;
namespace saved {
	static uint8_t r,g,b,a;
};

static uint8_t GREEN[] = {0,255,0};

#endif
