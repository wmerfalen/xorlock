#include <map>
#include <iostream>
#include <cmath>

#include "coordinates.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[COORD][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[COORD][ERROR]: " << A << "\n";
namespace coord {
	int get_angle(const int& src_x,const int& src_y, const int& dst_x, const int& dst_y) {
		static constexpr double PI = 3.14159265358979323846;
		int x=0,y=0;
		const int& px = src_x;
		const int& py = src_y;
    //if(src_x == dst_x && src_y > dst_y){
    //  // Facing exactly north
    //  return 271;
    //}
    //if(src_y == dst_y){
    //  // Facing exactly south
    //  return 271 + 180;
    //}
    
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
