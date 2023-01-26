#ifndef __BULLET_HEADER__
#define __BULLET_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <memory>
#include "actor.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "viewport.hpp"
#include "circle.hpp"
#include <vector>

namespace bullet {
	static Actor b;
	static constexpr double PI = 3.14159265358979323846;
	static Line line;
	static int radius;
	static SDL_Rect rect;
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
			bullets_index = 0;
			auto& r = bullets[bullets_index];
			r.line = _in_line;
			r.movement_amount = _in_movement_amount;

			for(const auto& p : r.line.points) {
				rect.x = p.x;
				rect.y = p.y;
				SDL_RenderCopy(
				    ren,
				    b.bmp[0].texture,
				    nullptr,
				    &rect);
			}
		}
		void init() {
			bullets_index = 0;
			for(std::size_t i=0; i < BULLET_SIZE_MAX; ++i) {
				bullets[i].line.p1 = {0,0};
				bullets[i].line.p2 = {0,0};
				bullets[i].movement_amount = 0;
			}
			rect.w = 10;
			rect.h = 10;
		}
	};
	static int pcx;
	static int pcy;

	void draw_shield() {
		save_draw_color();
		set_draw_color("red");
		shapes::DrawCircle(pcx,pcy,radius);
		restore_draw_color();
	}
	static constexpr int INITIAL_POINTS = 255;
	template <typename TPlayer>
	void fire_towards(TPlayer* p,const int& mouse_x, const int& mouse_y) {
		pcx = p->cx;
		pcy = p->cy;
		line.p1 = {p->cx,p->cy};
		line.p2.x = (10 * win_width()) * cos(PI * 2  * p->angle / 360);
		line.p2.y = (10 * win_height()) * sin(PI * 2 * p->angle / 360);
		line.getPoints(INITIAL_POINTS);
		std::vector<SDL_Point> circle_points = shapes::CaptureDrawCircle(p->cx, p->cy, radius);
		int closest = 999999;
		for(const auto& cp : circle_points) {
			for(const auto& pt : line.points) {
				int px = pt.x;
				int py = pt.y;
				int cx = cp.x;
				int cy = cp.y;
				int distance = sqrt(pow(px - cx,2) + pow(py - cy, 2) * 1.0);
				if(distance < closest) {
					line.p1.x = px;
					line.p1.y = py;
					closest = distance;
					if(distance < 2) {
						break;
					}
				}
			}
		}
		for(const auto& point : line.points) {
			int x = point.x;
			int y = point.y;
			if(x < viewport::min_x || x > viewport::max_x ||
			        y < viewport::min_y || y > viewport::max_y) {
				line.p2.x = x;
				line.p2.y = y;
				break;
			}
		}
		line.getPoints(INITIAL_POINTS);
		state::fire_bullet(p,line,150);
	}

	struct Point {
		int x;
		int y;
	};

	struct Bullet {
		SDL_Rect rect;
		Point src;
		Point dst;
		Point current;
		weapon_stats_t* stats;
		Line line;
		std::size_t line_index;
		int angle;

		bool done;
		Bullet() : done(true) {
			//std::cout << "Bullet() ";
		}
		Bullet(const Bullet&) = delete;
		~Bullet() = default;
		std::vector<SDL_Point> circle_points;
		void calc() {
			line_index = 0;
			static constexpr int INITIAL_POINTS = 600;
			static constexpr int radius = 55;
			angle = coord::get_angle();
			line.p1.x = src.x;
			line.p1.y = src.y;
			line.p2.x = (10 * win_width()) * cos(PI * 2  * angle / 360);
			line.p2.y = (10 * win_height()) * sin(PI * 2 * angle / 360);
			line.getPoints(INITIAL_POINTS);
			circle_points = shapes::CaptureDrawCircle(src.x,src.y, radius);
			int closest = 999999;
			for(const auto& cp : circle_points) {
				for(const auto& pt : line.points) {
					int px = pt.x;
					int py = pt.y;
					int cx = cp.x;
					int cy = cp.y;
					int distance = sqrt(pow(px - cx,2) + pow(py - cy, 2) * 1.0);
					if(distance < closest) {
						line.p1.x = px;
						line.p1.y = py;
						closest = distance;
						if(distance < 2) {
							break;
						}
					}
				}
			}
			for(const auto& point : line.points) {
				int x = point.x;
				int y = point.y;
				if(x < viewport::min_x || x > viewport::max_x ||
				        y < viewport::min_y || y > viewport::max_y) {
					line.p2.x = x;
					line.p2.y = y;
					break;
				}
			}
			line.getPoints(INITIAL_POINTS);
			rect.x = line.p1.x;
			rect.y = line.p1.y;
			rect.w = 10;
			rect.h = 10;
			current.x = line.p1.x;
			current.y = line.p1.y;
		}
		void travel() {

			if(line_index >= line.points.size()) {
				done = true;
				return;
			}
			for(; line_index < line.points.size(); ++line_index) {
				const auto& p = line.points[line_index];
				this->rect.x = p.x;
				this->rect.y = p.y;
				SDL_RenderCopy(
				    ren,
				    b.bmp[0].texture,
				    nullptr,
				    &this->rect);
				int distance = sqrt(pow(current.x - this->rect.x,2) + pow(current.y - this->rect.y, 2) * 1.0);
				if(distance >= (*stats)[WPN_PIXELS_PT]) {
					break;
				}
			}
			current.x = this->rect.x;
			current.y = this->rect.y;
		}

		void report() {
			std::cout << "Bullet: " << src.x << "x" << src.y <<
			          "=>" << dst.x << "x" << dst.y << "\n";
		}
	};
	struct BulletPool {
		static constexpr std::size_t POOL_SIZE = 1024;
		std::array<Bullet,POOL_SIZE> bullets;
		std::size_t index;
		~BulletPool() = default;
		BulletPool(const BulletPool&) = delete;
		BulletPool() : index(0) {};
		void queue(weapon_stats_t* stats_ptr) {
			if(index == POOL_SIZE) {
				index = 0;
			}
			auto& r = bullets[index];
			r.stats = stats_ptr;
			r.src.x = plr::get_cx();
			r.src.y = plr::get_cy();
			r.dst.x = cursor::mx();
			r.dst.y = cursor::my();
			r.calc();
			r.done = false;
			++index;
		}
	};
	static std::unique_ptr<BulletPool> pool;
	void queue_bullets(weapon_stats_t* stats_ptr) {
		pool->queue(stats_ptr);
	}
	void tick() {
		for(auto& bullet : pool->bullets) {
			if(bullet.done) {
				continue;
			}
			bullet.travel();
		}
	}
	void init() {
		b.x = 0;
		b.y = 0;
		b.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
		state::init();
		radius = 55;
		pool = std::make_unique<BulletPool>();
	}
};

#endif
