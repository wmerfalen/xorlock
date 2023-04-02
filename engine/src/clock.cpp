#include <SDL2/SDL.h>
#include "clock.hpp"

namespace clk {
	auto now() {
		return std::chrono::high_resolution_clock::now();
	}


};
