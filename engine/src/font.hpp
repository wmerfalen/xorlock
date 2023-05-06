#ifndef __FONT_HEADER__
#define __FONT_HEADER__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <array>


namespace font {
	void init();
	void quit();
	void draw_bubble_text(const SDL_Point* where,const std::string& msg);
};

#endif
