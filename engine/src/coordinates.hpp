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
	enum Facing : uint8_t {
		NORTH,
		NORTH_EAST,
		EAST,
		SOUTH_EAST,
		SOUTH,
		SOUTH_WEST,
		WEST,
		NORTH_WEST,
	};
	template <typename TPlayer>
	int get_gun_angle(const TPlayer& p,const int& mouse_x, const int& mouse_y) {
		int x=0,y=0;
		const int& px = p.gun_x;
		const int& py = p.gun_y;
		if(mouse_y <= py) {
			/**
			 * The mouse is to the north of quake guy
			 */
			y = py - mouse_y;
		}
		if(mouse_y >= py) {
			/**
			 * The mouse is to the south of quake guy
			 */
			y = - 1 * (mouse_y - py);
		}

		if(mouse_x <= px) {
			/**
			 * The mouse is to the west of quake guy
			 */
			x = mouse_x - px;
		}
		if(mouse_x >= px) {
			/**
			 * The mouse is to the east of quake guy
			 */
			x = mouse_x - px;
		}
		return (atan2(x,y) * (180 / PI)) + 270;
	}
	template <typename TPlayer>
	int get_angle(const TPlayer& p,const int& mouse_x, const int& mouse_y) {
		int x=0,y=0;
		const int& px = p.cx;
		const int& py = p.cy;
		if(mouse_y <= py) {
			/**
			 * The mouse is to the north of quake guy
			 */
			y = py - mouse_y;
		}
		if(mouse_y >= py) {
			/**
			 * The mouse is to the south of quake guy
			 */
			y = - 1 * (mouse_y - py);
		}

		if(mouse_x <= px) {
			/**
			 * The mouse is to the west of quake guy
			 */
			x = mouse_x - px;
		}
		if(mouse_x >= px) {
			/**
			 * The mouse is to the east of quake guy
			 */
			x = mouse_x - px;
		}
		return (atan2(x,y) * (180 / PI)) + 270;
	}
	int get_angle() {
		int x=0,y=0;
		const int px = plr::get_cx();
		const int py = plr::get_cy();
		const int mouse_x = cursor::mx();
		const int mouse_y = cursor::my();
		if(mouse_y <= py) {
			/**
			 * The mouse is to the north of quake guy
			 */
			y = py - mouse_y;
		}
		if(mouse_y >= py) {
			/**
			 * The mouse is to the south of quake guy
			 */
			y = - 1 * (mouse_y - py);
		}

		if(mouse_x <= px) {
			/**
			 * The mouse is to the west of quake guy
			 */
			x = mouse_x - px;
		}
		if(mouse_x >= px) {
			/**
			 * The mouse is to the east of quake guy
			 */
			x = mouse_x - px;
		}
		return (atan2(x,y) * (180 / PI)) + 270;
	}
};

#endif
