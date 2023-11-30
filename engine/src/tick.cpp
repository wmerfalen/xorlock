#include <iostream>
#include <SDL2/SDL.h>
#include "tick.hpp"

namespace tick {
	tick_t get() {
		return SDL_GetTicks();
	}

};
