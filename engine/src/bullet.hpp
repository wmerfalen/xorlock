#ifndef __BULLET_HEADER__
#define __BULLET_HEADER__
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

namespace bullet {
	static Actor b;
	static constexpr double PI = 3.14159265358979323846;
	static Line line;
	static int radius;
	void cleanup_pool();

	struct Point {
		int x;
		int y;
	};


	struct Bullet {
		static constexpr int INITIAL_POINTS = 96;
		static constexpr int radius = 55;
		uint32_t start_tick;
		SDL_Rect rect;
		Point src;
		Point dst;
		Point current;
		weapon_stats_t* stats;
		Line line;
		std::size_t line_index;
		int angle;

		std::vector<Point> trimmed;
		std::vector<SDL_Point> circle_points;
		int distance;
		int closest;
		bool done;
		bool initialized;
		Bullet() : done(false) {
			line_index = 0;
			rect.w = 20;
			rect.h = 20;
			initialized = true;
		}
		Bullet(const Bullet& o) = delete;
		~Bullet() = default;
		void calc() {
			start_tick = tick::get();
			distance = closest = 9999;
			line_index = 0;
			angle = coord::get_angle();
			line.p1.x = src.x;
			line.p1.y = src.y;
			line.p2.x = (1000 * win_width()) * cos(PI * 2  * angle / 360);
			line.p2.y = (1000 * win_height()) * sin(PI * 2 * angle / 360);
			line.getPoints(INITIAL_POINTS);
			circle_points = shapes::CaptureDrawCircle(src.x,src.y, radius);
			for(const auto& cp : circle_points) {
				for(const auto& pt : line.points) {
					distance = sqrt(pow(pt.x - cp.x,2) + pow(pt.y - cp.y, 2) * 1.0);
					if(distance < closest) {
						line.p1.x = pt.x;
						line.p1.y = pt.y;
						closest = distance;
						if(distance < 60) {
							break;
						}
					}
				}
			}
			for(const auto& point : line.points) {
				if(point.x < viewport::min_x || point.x > viewport::max_x ||
				        point.y < viewport::min_y || point.y > viewport::max_y) {
					line.p2.x = point.x;
					line.p2.y = point.y;
					break;
				}
			}
			line.getPoints(INITIAL_POINTS);
			rect.x = line.p1.x;
			rect.y = line.p1.y;
			current.x = line.p1.x;
			current.y = line.p1.y;
			for(const auto& p : line.points) {
				distance = sqrt(pow(current.x - p.x,2) + pow(current.y - p.y, 2) * 1.0);
				if(distance >= (*stats)[WPN_PIXELS_PT]) {
					trimmed.emplace_back();
					auto& r = trimmed.back();
					r.x = p.x;
					r.y = p.y;
					current.x = p.x;
					current.y = p.y;
				}
			}
			current.x = line.p1.x;
			current.y = line.p1.y;
			line.points.clear();
			initialized = true;
		}
		void clear_out() {
			done = true;
			initialized = false;
			trimmed.clear();
			line.points.clear();
		}
		bool needs_processing() {
			return !done && initialized;
		}
		void travel() {
			if(line_index >= trimmed.size() - 1) {
				clear_out();
				return;
			}
			rect.x = trimmed[line_index].x;
			rect.y = trimmed[line_index].y;
			SDL_RenderCopy(
			    ren,
			    b.bmp[0].texture,
			    nullptr,
			    &rect);
			current.x = rect.x;
			current.y = rect.y;
			++line_index;
		}

		void report() {
			std::cout << "Bullet: " << src.x << "x" << src.y <<
			          "=>" << dst.x << "x" << dst.y << "\n";
		}
	};
	struct BulletPool {
		static constexpr std::size_t POOL_SIZE = 16;
		std::size_t index;
		std::array<std::unique_ptr<Bullet>,POOL_SIZE> bullets;
		~BulletPool() = default;
		BulletPool(const BulletPool&) = delete;
		BulletPool()  {
			for(std::size_t i=0; i < POOL_SIZE; ++i) {
				bullets[i] = std::make_unique<Bullet>();
				bullets[i]->clear_out();
			}
		};
		void queue(weapon_stats_t* stats_ptr) {
			if(index >= POOL_SIZE -1) {
				index = 0;
			}

			auto& r = this->bullets[index];
			r->stats = stats_ptr;
			r->src.x = plr::get_cx();
			r->src.y = plr::get_cy();
			r->dst.x = cursor::mx();
			r->dst.y = cursor::my();
			r->calc();
			r->done = false;
			r->initialized = true;
			++index;
		}
		void cleanup() {
		}
	};
	static std::unique_ptr<BulletPool> pool;
	void queue_bullets(weapon_stats_t* stats_ptr) {
		pool->queue(stats_ptr);
	}
	void tick() {
		for(auto& bullet : pool->bullets) {
			if(bullet->needs_processing()) {
				if(bullet->start_tick + 600 > tick::get()) {
					bullet->travel();
				} else {
					bullet->clear_out();
				}
			}
		}
	}
	void init() {
		b.x = 0;
		b.y = 0;
		b.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
		radius = 55;
		pool = std::make_unique<BulletPool>();
	}
	void cleanup_pool() {
	}
};

#endif
