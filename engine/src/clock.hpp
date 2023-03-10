#ifndef __CLOCK_HEADER__
#define __CLOCK_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace clk {
	auto now() {
		return std::chrono::high_resolution_clock::now();
	}


};

#endif
