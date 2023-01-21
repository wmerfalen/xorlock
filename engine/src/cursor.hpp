#ifndef __CURSOR_HEADER__
#define __CURSOR_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include "extern.hpp"


namespace cursor {
	SDL_Cursor* ptr = nullptr;
	SDL_Surface* surface = nullptr;
	int mouse_x;
	int mouse_y;
	void init() {
#ifdef USE_CURSOR
		surface = SDL_LoadBMP("../assets/reticle-0.bmp");
		ptr = SDL_CreateColorCursor(surface,42,42);
		SDL_SetCursor(ptr);
#else
		SDL_ShowCursor(SDL_DISABLE);
#endif
	}
	void update_mouse(const int& _mx, const int& _my) {
		mouse_x = _mx;
		mouse_y = _my;
	}
};


#endif