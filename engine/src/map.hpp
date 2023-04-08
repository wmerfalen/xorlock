#ifndef __MAP_HEADER__
#define __MAP_HEADER__

#include "actor.hpp"
#include "world.hpp"
#include "coordinates.hpp"
#include "viewport.hpp"
#include "circle.hpp"
#include "rng.hpp"
#include "draw.hpp"
#include <vector>
#include <functional>
#include <memory>

namespace map {
	void tick();
	void init();
};

#endif
