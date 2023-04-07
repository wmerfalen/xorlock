#include "direction.hpp"

namespace dir {
	Direction get_facing(int angle) {
		if(angle <= 450 - HALF_RADIAN && angle >= 405 - HALF_RADIAN) {
			/** Facing SOUTH EAST */
			return SOUTH_EAST;
		} else if(angle >= 360 - (HALF_RADIAN) && angle <= 360 + (HALF_RADIAN)) {
			/** FACING EAST */
			return EAST;
		} else if(angle >= 315 - HALF_RADIAN && angle <= 360 - HALF_RADIAN) {
			/** FACING NORTH EAST */
			return NORTH_EAST;
		} else if(angle >= 270 - HALF_RADIAN && angle <= 315 - HALF_RADIAN) {
			/** FACING NORTH */
			return NORTH;
		} else if(angle >= 225 - HALF_RADIAN && angle <= 270 - HALF_RADIAN) {
			/** FACING NORTH WEST */
			return NORTH_WEST;
		} else if(angle >= 180 - HALF_RADIAN && angle <= 225 - HALF_RADIAN) {
			/** FACING WEST */
			return WEST;
		} else if(angle >= 135 - HALF_RADIAN && angle <= 180 - HALF_RADIAN) {
			/** FACING SOUTH WEST */
			return SOUTH_WEST;
		} else {
			/** FACING SOUTH */
			return SOUTH;
		}
	}
};
