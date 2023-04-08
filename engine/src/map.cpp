#include "direction.hpp"
#include "map.hpp"
#include "world.hpp"
#include "player.hpp"
#include <functional>
#include "wall.hpp"

namespace map {
	SDL_Rect collision;

	void tick() {
		wall::tick();
	}
	void init() {
		wall::init();
	}
};
