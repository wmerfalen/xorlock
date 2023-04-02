#include <SDL2/SDL.h>
#include <iostream>
#include "gameplay.hpp"

namespace gameplay {
	void init() {
		npc_spawning::init();
	}
	void tick() {
		std::cout << "gameplay::tick()\n";
	}
}; // end namespace gameplay
