#ifndef __CONSTANTS_HEADER__
#define __CONSTANTS_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <memory>
#include <iomanip>
#include "actor.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "viewport.hpp"
#include "circle.hpp"
#include <vector>
#include <deque>
#include "clock.hpp"
#include "rng.hpp"
#include "draw.hpp"
#include "line.hpp"
#include "npc-id.hpp"


namespace constants {
	static constexpr int	BULLET_TRAIL_HEIGHT =  20;
	static constexpr int BULLET_TRAIL_WIDTH = 5;
};

#endif
