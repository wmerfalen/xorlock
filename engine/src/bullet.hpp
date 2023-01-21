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
	namespace state {
		struct Bullet {
			Line line;
			int movement_amount;
		};
		static constexpr std::size_t BULLET_SIZE_MAX = 1024;
		static std::array<Bullet,BULLET_SIZE_MAX> bullets;
		static std::size_t bullets_index;
		bool near_distance(int dist, int move, int give_or_take) {
			if(dist == move) {
				return true;
			}
			if(dist + give_or_take >= move && dist + give_or_take < move + give_or_take) {
				return true;
			}
			return false;
		}
		template <typename TPlayer>
		void fire_bullet(TPlayer* plr,const Line& _in_line,int _in_movement_amount) {
			auto& r = bullets[bullets_index];
			r.line = _in_line;
			r.movement_amount = _in_movement_amount;
			r.line.points.clear();
			r.line.getPoints(256);

			SDL_Rect rect;
			rect.w = 10;
			rect.h = 10;
			std::vector<Point> where;
			int multiplier = 1;
			for(const auto& p : r.line.points) {
				auto distance = sqrt(pow(plr->cx - p.x,2) + pow(plr->cy - p.y, 2) * 1.0);
				if(near_distance(distance,r.movement_amount * multiplier,10)) {
					where.emplace_back(p);
					++multiplier;
				}
			}
			for(const auto& p : r.line.points) {
				const auto& w = where[0];
				if(p.x == w.x && p.y == w.y) {
					break;
				}
				rect.w = 5;
				rect.h = 2;
				rect.x = p.x;
				rect.y = p.y;
				SDL_RenderCopy(
				    ren,
				    b.bmp[0].texture,
				    nullptr,
				    &rect
				);
			}
			for(const auto& p : where) {
				rect.x = p.x;
				rect.y = p.y;
				SDL_RenderCopy(
				    ren,
				    b.bmp[0].texture,
				    nullptr,
				    &rect
				);
			}
			//++bullets_index;
		}
		void init() {
			bullets_index = 0;
			for(std::size_t i=0; i < BULLET_SIZE_MAX; ++i) {
				bullets[i].line.p1 = {0,0};
				bullets[i].line.p2 = {0,0};
				bullets[i].movement_amount = 0;
			}
		}
	};

	void init() {
		b.x = 0;
		b.y = 0;
		b.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
		state::init();
	}
	template <typename TPlayer>
	void fire_towards(TPlayer* p,const int& mouse_x, const int& mouse_y) {
		line.p1.x = p->cx;
		line.p1.y = p->cy;
		line.angle = p->angle = coord::get_angle(*p,mouse_x,mouse_y);
		line.p2.x = (100 * win_width()) * cos(PI * 2  * p->angle / 360);
		line.p2.y = (100 * win_height()) * sin(PI * 2 * p->angle / 360);
		auto points  = line.getPoints(512);
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
		state::fire_bullet(p,line,150);
	}

	void tick() {

	}

};


#endif
