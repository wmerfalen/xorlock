#ifndef __BULLET_HEADER__
#define __BULLET_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "actor.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include <map>
#include "extern.hpp"
#include "viewport.hpp"

namespace bullet {
	static Actor b;
	static constexpr double PI = 3.14159265358979323846;
	static Line line;
	void init() {
		b.x = 0;
		b.y = 0;
		b.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
	}

	template <typename TPlayer>
	void fire_towards(TPlayer* p,const int& mouse_x, const int& mouse_y) {
		line.p1.x = p->cx;
		line.p1.y = p->cy;
		line.angle = p->angle = coord::get_angle(*p,mouse_x,mouse_y);
		line.p2.x = 2500 * cos(PI * 2  * p->angle / 360);
		line.p2.y = 2500 * sin(PI * 2 * p->angle / 360);
		SDL_Rect bullet_rect;
		bullet_rect.w = 10;
		bullet_rect.h = 10;
		auto points  = line.getPoints(224);
		for(int i=0; i < points.size(); i++) {
			if(points[i].x < 0 || points[i].y < 0) {
				line.p2.x = points[i].x;
				line.p2.y = points[i].y;
				break;
			}
			if(points[i].x < viewport::min_x || points[i].x > viewport::max_x ||
			        points[i].y < viewport::min_y || points[i].y > viewport::max_y) {
				line.p2.x = points[i].x;
				line.p2.y = points[i].y;
				break;
			}
		}
		for(const auto& point : line.getPoints(224)) {
			bullet_rect.x = point.x;
			bullet_rect.y = point.y;
			SDL_RenderCopy(
			    ren,
			    b.bmp[0].texture,
			    nullptr,
			    &bullet_rect
			);
		}
	}

	void tick() {

	}

};


#endif
