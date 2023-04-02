#ifndef __DRAW_HEADER__
#define __DRAW_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "extern.hpp"

namespace draw {
	void axis(int angle,uint8_t color[]);
	void grid();
	void bullet_line(int x, int y,int tox,int toy);
	void draw_green();
	void restore_color();
	void draw_red();

	void line(int x, int y,int tox,int toy);
};

#endif
