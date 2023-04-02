#ifndef __COORD_HEADER__
#define __COORD_HEADER__
#include <map>
#include <iostream>
#include <cmath>
#include "extern.hpp"

/**
 * Goal:
 * Coordinates need to be adjusted so that quake guy is (0,0).
 * The problem is that quake guy is almost never at (0,0). He's always
 * at something like 512x512. In order to allow us to detect the angle
 * of the mouse relative to quake guy, we must translate the mouse x,y coordinates
 * relative to quake guy.
 */
namespace coord {
	static constexpr double PI = 3.14159265358979323846;
	int get_angle(const int& src_x,const int& src_y, const int& dst_x, const int& dst_y) {
		int x=0,y=0;
		const int& px = src_x;
		const int& py = src_y;
		if(dst_y <= py) {
			/**
			 * The mouse is to the north of quake guy
			 */
			y = py - dst_y;
		}
		if(dst_y >= py) {
			/**
			 * The mouse is to the south of quake guy
			 */
			y = - 1 * (dst_y - py);
		}

		if(dst_x <= px) {
			/**
			 * The mouse is to the west of quake guy
			 */
			x = dst_x - px;
		}
		if(dst_x >= px) {
			/**
			 * The mouse is to the east of quake guy
			 */
			x = dst_x - px;
		}
		return (atan2(x,y) * (180 / PI)) + 270;
	}
};

#endif
