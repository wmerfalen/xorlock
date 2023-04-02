#ifndef __VIEWPORT_HEADER__
#define __VIEWPORT_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include "extern.hpp"


namespace viewport {
	static int min_x;
	static int max_x;
	static int min_y;
	static int max_y;
	void init() {
		min_x = 0;
		max_x = win_width();
		min_y = 0;
		max_y = win_height();
	}
	std::string report() {
		std::string s = "viewport: ";
		s += "min_x: ";
		s += std::to_string(min_x);
		s += "max_x: ";
		s += std::to_string(max_x);
		s += "min_y: ";
		s += std::to_string(min_y);
		s += "max_y: ";
		s += std::to_string(max_y);
		return s;
	}
};


#endif
