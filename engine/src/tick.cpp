#include <iostream>
#include <SDL2/SDL.h>
#include "tick.hpp"

uint64_t CURRENT_TICK = 0;
namespace tick {
	tick_t get() {
		return CURRENT_TICK = SDL_GetTicks64();
	}

};
