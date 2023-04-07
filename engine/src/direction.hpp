#ifndef __DIRECTION_HEADER__
#define __DIRECTION_HEADER__
#include <iostream>

enum Direction : uint8_t {
	NORTH,
	EAST,
	WEST,
	SOUTH,
	NORTH_EAST,
	NORTH_WEST,
	SOUTH_EAST,
	SOUTH_WEST,
};
namespace dir {
	static constexpr int HALF_RADIAN = 45 / 2;
	Direction get_facing(int angle);
};

#endif

