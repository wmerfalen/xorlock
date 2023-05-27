#ifndef __COLOR_HEADER__
#define __COLOR_HEADER__
#include <iostream>
#include <SDL2/SDL.h>
#include "window.hpp"

namespace colors {
	uint8_t* green() ;
	uint8_t* red();
	uint8_t* blue();
	uint8_t* bullet_line();
	namespace sdl {
		SDL_Color green();
		SDL_Color red();
		SDL_Color blue();
	};
};

void save_draw_color();
void set_draw_color(const char*);
void restore_draw_color();

#endif
