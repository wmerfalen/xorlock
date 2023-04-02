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
	void init();
	void use_reticle();
	void disable_cursor();
	void update_mouse();
	int mx();
	int my();
};


#endif
