#include <SDL2/SDL.h>
#include <iostream>
#include "paths.hpp"
#include "../player.hpp"
#include "../direction.hpp"
#include "../draw.hpp"
#include "../wall.hpp"
#include "../movement.hpp"
#include <cmath>
#include <tuple>
#include <map>
#include <array>
#include <algorithm>

namespace wall {
	extern std::vector<Wall*> blockable_walls;
};
namespace npc::paths {
	static constexpr int PATH_DIVISOR = 50;

	namespace calc {
		auto distance(int32_t x1, int32_t y1, int32_t x2,int32_t y2) {
			auto dx{x1 - x2};
			auto dy{y1 - y2};
			return std::sqrt(dx*dx + dy*dy);
		}
	};
	using point_t = std::tuple<std::pair<int32_t,int32_t>,bool>;
	static constexpr std::size_t DEMO_POINTS_SIZE = 10 * 1024;
	std::array<point_t,DEMO_POINTS_SIZE> demo_points;
	using Coordinate = vpair_t;

	std::vector<vpair_t> getCoordinates(const vpair_t& point1, const vpair_t& point2, int distance) {
		std::vector<vpair_t> coordinates;

		// Calculate the differences in x and y coordinates
		int dx = point2.first - point1.first;
		int dy = point2.second - point1.second;

		// Calculate the total number of steps needed
		int numSteps = std::max(std::abs(dx), std::abs(dy));
		numSteps = std::min((std::size_t)numSteps,DEMO_POINTS_SIZE);

		std::cout << "numSteps:" << numSteps << "\n";
		// Calculate the step size for each coordinate
		double stepSizeX = static_cast<double>(dx) / numSteps;
		double stepSizeY = static_cast<double>(dy) / numSteps;

		// Generate the coordinates
		for(int i = 0; i <= numSteps; i++) {
			vpair_t coordinate;

			// Calculate the x and y values for the current step
			coordinate.first = static_cast<int>(point1.first + i * stepSizeX * (distance / 2));
			coordinate.second = static_cast<int>(point1.second + i * stepSizeY * (distance / 2));

			coordinates.push_back(coordinate);
			if(calc::distance(coordinate.first,coordinate.second,point2.first,point2.second) <= CELL_WIDTH) {
				return coordinates;
			}
		}

		return coordinates;
	}
	void PathFinder::crawl_map_tiles() {
		//heading = calculate_heading();
		//std::cout << dir::to_string(heading) << "\n";
		//auto line = getCoordinates(start_point,destination_point,CELL_WIDTH);
		//for(auto& l : demo_points) {
		//	l = {{0,0},false};
		//}
		//std::size_t i = 0;
		//for(const auto& pixel : line) {
		//	demo_points[i++] = {pixel,true};
		//	if(i >= DEMO_POINTS_SIZE) {
		//		break;
		//	}
		//}
	}
	void PathFinder::draw_path() {

	}
	void PathFinder::update(Actor* _in_hunter,Actor* _in_target) {
		x = _in_hunter->rect.x;
		y = _in_hunter->rect.y;
		target_x = _in_target->rect.x;
		target_y = _in_target->rect.y;

		start_point = {x,y};
		destination_point = {target_x,target_y};
		heading = calculate_heading();
		//crawl_map_tiles();
	}

	PathFinder::PathFinder(int hunter_movement_amount, Actor* _in_hunter,Actor* _in_target)
		:
		movement_amount(hunter_movement_amount),
		x(_in_hunter->rect.x),
		y(_in_hunter->rect.y),
		target_x(_in_target->rect.x),
		target_y(_in_target->rect.y) {

	}
	bool PathFinder::is_obstacle(const SDL_Point* _p) {
		return std::find(obstacles.cbegin(),obstacles.cend(),_p) != obstacles.cend();
	}
	Direction PathFinder::calculate_heading() {
		Direction h = NORTH;
		bool n,s,e,w;
		n = s = e = w = 0;

		if(x <= target_x) {
			e = true;
			h = EAST;
		} else {
			w = true;
			h = WEST;
		}
		if(y <= target_y) {
			s = true;
			h = SOUTH;
		} else {
			n = true;
			h = NORTH;
		}
		if(e) {
			if(n) {
				return NORTH_EAST;
			}
			if(s) {
				return SOUTH_EAST;
			}
		}
		if(w) {
			if(n) {
				return NORTH_WEST;
			}
			if(s) {
				return SOUTH_WEST;
			}
		}
		return h;
	}

	void PathFinder::animate() {
#ifdef SHOW_PATHFINDER_ANIMATE
		SDL_Point player {target_x,target_y};
		for(const auto& p : points) {
			draw::hires_line(&p,&player);
		}
#endif
	}
};
