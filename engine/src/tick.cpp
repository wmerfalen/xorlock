#include <iostream>
#include "tick.hpp"

namespace tick {
	uint64_t get() {
		return SDL_GetTicks();
	}
};
