#ifndef __TICK_HEADER__
#define __TICK_HEADER__
#include <iostream>
#include "extern.hpp"

namespace tick {
	auto get() {
		return SDL_GetTicks();
	}
};

#endif
