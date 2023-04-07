#ifndef __COLOR_HEADER__
#define __COLOR_HEADER__
#include <iostream>
#include <SDL2/SDL.h>
#include "color.hpp"
#include "window.hpp"

namespace colors {
	static uint8_t g[] = {0,255,0};
	static uint8_t _red[] = {255,0,0};
	static uint8_t _blue[] = {0,0,255};
	uint8_t* green() {
		return g;
	}
	uint8_t* red() {
		return _red;
	}
	uint8_t* blue() {
		return _blue;
	}
	uint8_t _bullet_line[] = {255, 255, 204};
	uint8_t* bullet_line() {
		return _bullet_line;
	}
};
namespace saved {
	static uint8_t r,g,b,a;
};

bool between(int target, int min,int max) {
	return target > min && target < max;
}
void save_draw_color() {
	using namespace saved;
	SDL_GetRenderDrawColor(ren,&r,&g,&b,&a);
}
void restore_draw_color() {
	using namespace saved;
	SDL_SetRenderDrawColor(ren,r,g,b,a);
}
void set_draw_color(const char* s) {
	if(strcmp("red",s) == 0) {
		SDL_SetRenderDrawColor(ren,255,0,0,0);
	}
}
#endif
