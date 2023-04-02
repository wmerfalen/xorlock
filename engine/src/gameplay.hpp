#ifndef __GAMEPLAY_HEADER__
#define __GAMEPLAY_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "extern.hpp"
#include "npc-spetsnaz.hpp"
#include "gameplay/npc-spawning.hpp"

namespace gameplay {
	void init() {
		npc_spawning::init();
	}
	void tick() {
		std::cout << "gameplay::tick()\n";
	}
}; // end namespace gameplay


#endif
