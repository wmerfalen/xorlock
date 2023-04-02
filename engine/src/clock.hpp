#ifndef __CLOCK_HEADER__
#define __CLOCK_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>

namespace clk {
	std::chrono::time_point<std::chrono::high_resolution_clock> now();

};

#endif
