#ifndef __GAMEPLAY_HEADER__
#define __GAMEPLAY_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"

#include "npc-spetsnaz.hpp"
#include "gameplay/npc-spawning.hpp"

namespace gameplay {
	void init();
	void tick();
}; // end namespace gameplay


#endif
