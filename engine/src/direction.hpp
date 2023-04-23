#ifndef __DIRECTION_HEADER__
#define __DIRECTION_HEADER__
#include <iostream>

enum Direction : uint8_t {
	NORTH = (1 << 0),
	EAST = (1 << 1),
	WEST = (1 << 2),
	SOUTH = (1 << 3),
	NORTH_EAST = (1 << 4),
	NORTH_WEST = (1 << 5),
	SOUTH_EAST = (1 << 6),
	SOUTH_WEST = (1 << 7),
};
namespace dir {
	static constexpr int HALF_RADIAN = 45 / 2;
	Direction get_facing(int angle);
	std::string to_string(int d);
};

#endif

