#ifndef __CURSOR_HEADER__
#define __CURSOR_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include "extern.hpp"


namespace cursor {
	static SDL_Cursor* ptr = nullptr;
	static SDL_Surface* surface = nullptr;
	static int mouse_x;
	static int mouse_y;
	void init() {
	}
	void use_reticle() {
		surface = SDL_LoadBMP("../assets/reticle-0.bmp");
		ptr = SDL_CreateColorCursor(surface,42,42);
		SDL_SetCursor(ptr);
	}
	void disable_cursor() {
		SDL_ShowCursor(SDL_DISABLE);
	}
	void update_mouse(const int& _mx, const int& _my) {
		mouse_x = _mx;
		mouse_y = _my;
	}
	int mx() {
		return mouse_x;
	}
	int my() {
		return mouse_y;
	}
};


#endif
