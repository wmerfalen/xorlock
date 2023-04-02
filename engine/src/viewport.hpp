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
	void init();
	std::string report();
};


#endif
