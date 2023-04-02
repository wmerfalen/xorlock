#include <SDL2/SDL.h>
#include "clock.hpp"
#include <chrono>
#include <iomanip>

namespace clk {
	std::chrono::time_point<std::chrono::high_resolution_clock> now() {
		return std::chrono::high_resolution_clock::now();
	}


};
