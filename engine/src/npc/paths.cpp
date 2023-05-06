#include <SDL2/SDL.h>
#include <iostream>
#include "paths.hpp"
#include "../player.hpp"
#include "../direction.hpp"
#include "../draw.hpp"
#include "../wall.hpp"
#include "../movement.hpp"

namespace wall {
	extern std::vector<Wall*> blockable_walls;
};
namespace npc::paths {
	static constexpr int PATH_DIVISOR = 50;
	void PathFinder::update(Actor* _in_hunter,Actor* _in_target) {
		x = _in_hunter->rect.x;
		y = _in_hunter->rect.y;
		target_x = _in_target->rect.x;
		target_y = _in_target->rect.y;

		if(x < target_x) {
			x_distance = target_x - x;
		} else {
			x_distance = x - target_x;
		}
		if(y < target_y) {
			y_distance = target_y - y;
		} else {
			y_distance = y - target_y;
		}
		x_distance /= PATH_DIVISOR;
		y_distance /= PATH_DIVISOR;
		start_point = {x,y};
		destination_point = {target_x,target_y};
		m_grid.resize(x_distance);
		for(int i=0; i < x_distance; i++) {
			m_grid[i].resize(y_distance);
		}
		std::cout << "start_point: " << x << "x" << y << "\n";
		std::cout << "destination_point: " << target_x << "x" << target_y << "\n";
		//std::cout << "m_sample_grid: " << m_sample_grid.size() << "\n";
	}
	auto distance(int32_t x1, int32_t y1, int32_t x2,int32_t y2) {
		auto dx{x1 - x2};
		auto dy{y1 - y2};
		return std::sqrt(dx*dx + dy*dy);
	}
	bool within_reach(const int32_t& x,const int32_t& y,const int32_t& target_x,const int32_t& target_y,const int32_t& path_divisor) {
		static std::map<int,std::size_t> calls;
		auto d = distance(x,y,target_x,target_y) / path_divisor;
		// FIXME
		if(calls[d]++ >= 80) {
			calls.clear();
			return true;
		}
		return false;
	}
	const std::size_t& PathFinder::calculate_path() {
		int x_multiplier = 1;
		int y_multiplier = 1;
		if(x >= target_x) {
			x_multiplier = -1;
		}
		if(y >= target_y) {
			y_multiplier = -1;
		}
		for(int xx=0; xx < x_distance; xx++) {
			for(int yy=0; yy < y_distance; yy++) {
				m_grid[xx][yy].x = x + (x_multiplier * (xx * PATH_DIVISOR));
				m_grid[xx][yy].y = y + (y_multiplier * (yy * PATH_DIVISOR));
				m_grid[xx][yy].weight = -1;
				m_grid[xx][yy].visited = false;
				for(int i=0; i < 8; i++) {
					m_grid[xx][yy].neighbors[i] = nullptr;
				}
				//std::cout << m_grid[xx][yy].x << "x" << m_grid[xx][yy].y << " ";
			}
			//std::cout << "\n";
		}

		std::vector<Score*> optimal_path;
		/**
		 * Start crawl
		 */
		{
			auto& start = m_grid[0][0];
			optimal_path.clear();
			for(int rows=0; rows < x_distance; rows++) {
				for(int columns=0; columns < y_distance; columns++) {
					auto& current = m_grid[rows][columns];
					current.distance = distance(target_x,target_y,current.x,current.y);
					//std::cout << rows << "x" << columns << " dir: ";
					if(rows - 1 >= 0 && columns - 1 >= 0) {
						current.neighbors[NEG_NORTH_WEST] = &m_grid[rows-1][columns-1];
						//std::cout << "NEG_NORTH_WEST";
					} else {
						//std::cout << "!NEG_NORTH_WEST";
						current.neighbors[NEG_NORTH_WEST] = nullptr;
					}
					if(rows - 1 >= 0) {
						current.neighbors[NEG_NORTH] = &m_grid[rows-1][columns];
						//std::cout << "NEG_NORTH";
					} else {
						current.neighbors[NEG_NORTH] = nullptr;
						//std::cout << "!NEG_NORTH";
					}
					if(rows - 1 >= 0 && columns + 1 < y_distance) {
						current.neighbors[NEG_NORTH_EAST] = &m_grid[rows-1][columns+1];
						////std::cout << "NEG_NORTH_EAST";
					} else {
						current.neighbors[NEG_NORTH_EAST] = nullptr;
						//std::cout << "!NEG_NORTH_EAST";
					}
					if(columns + 1 < y_distance) {
						current.neighbors[NEG_EAST] = &m_grid[rows][columns+1];
						//std::cout << "NEG_EAST";
					} else {
						current.neighbors[NEG_EAST] = nullptr;
						//std::cout << "!NEG_EAST";
					}
					if(columns + 1 < y_distance && rows + 1 < x_distance) {
						current.neighbors[NEG_SOUTH_EAST] = &m_grid[rows+1][columns+1];
						//std::cout << "NEG_SOUTH_EAST";
					} else {
						current.neighbors[NEG_SOUTH_EAST] = nullptr;
						//std::cout << "!NEG_SOUTH_EAST";
					}
					if(rows + 1 < x_distance) {
						current.neighbors[NEG_SOUTH] = &m_grid[rows+1][columns];
						//std::cout << "NEG_SOUTH";
					} else {
						current.neighbors[NEG_SOUTH] = nullptr;
						//std::cout << "!NEG_SOUTH";
					}
					if(rows - 1 >= 0 && columns + 1 < y_distance) {
						current.neighbors[NEG_SOUTH_WEST] = &m_grid[rows-1][columns+1];
						//std::cout << "NEG_SOUTH_WEST";
					} else {
						current.neighbors[NEG_SOUTH_WEST] = nullptr;
						//std::cout << "!NEG_SOUTH_WEST";
					}
					if(rows - 1 >= 0) {
						current.neighbors[NEG_WEST] = &m_grid[rows-1][columns];
						//std::cout << "NEG_WEST";
					} else {
						current.neighbors[NEG_WEST] = nullptr;
						//std::cout << "!NEG_WEST";
					}
					//std::cout << "\n";
				}
			}
			int dist_to_goal = 999;
			int choice = -1;
			auto ptr = &start;
			bool dist_found = false;
			optimal_path.emplace_back(ptr);
			std::vector<int> last_couple_distances;
			do {
				dist_found = false;
				dist_to_goal = 999999;
				choice = -1;
				for(int i=0; i < 8; i++) {
					if(ptr->neighbors[i] == nullptr) {
						continue;
					}
					//std::cout << "target_x: " << target_x << "x" << target_y <<
					//          "px:" << ptr->neighbors[i]->x << "py:" << ptr->neighbors[i]->y << "\n";
					int dist = distance(target_x,target_y,ptr->neighbors[i]->x,ptr->neighbors[i]->y);
					//std::cout << "dist: " << dist << " current:" << dist_to_goal << "\n";
					if(dist < dist_to_goal) {
						//std::cout << "winner: " << i << "\n";
						dist_to_goal = dist;
						dist_found = true;
						choice = i;
						continue;
					}
				}
				//std::cout << "choice: " << choice << "\n";
				if(choice == -1) {
					//std::cout << "cant find choice\n";
					continue;
				}
				if(!dist_found) {
					//std::cout << "dist_to_goal failed\n";
					continue;
				}
				optimal_path.emplace_back(ptr);
				ptr = ptr->neighbors[choice];
			} while(ptr && !within_reach(ptr->x,ptr->y,target_x,target_y,PATH_DIVISOR));
			valid_nodes = std::min(POINTS,optimal_path.size());
			std::size_t i =0;
			for(const auto& op : optimal_path) {
				points[i].x = op->x;
				points[i].y = op->y;
				++i;
			}
		}

		{
			for(int xx =0; xx < x_distance; xx++) {
				bool found = false;
				for(int yy=0; yy < y_distance; yy++) {
					found = 0;
					for(auto& e : optimal_path) {
						if(!e) {
							//std::cout << "invalid e\n";
							continue;
						}
						if(e->x == (x + (x_multiplier * xx * PATH_DIVISOR)) && e->y == (y + (y_multiplier * yy * PATH_DIVISOR))) {
							found = true;
							break;
						}
					}
					if(found) {
						std::cout << "x";
					} else {
						std::cout << ".";
					}
				}
				std::cout << "\n";
			}
		}

		return valid_nodes;
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
		uint8_t h = 0;
		bool n,s,e,w;
		n = s = e = w = 0;

		if(x < target_x) {
			e = true;
		} else {
			w = true;
		}
		if(y < target_y) {
			n = true;
		} else {
			s = true;
		}
		if(e) {
			h = EAST;
			if(n) {
				h = (uint8_t)NORTH | (uint8_t)EAST;
			}
			if(s) {
				h = (uint8_t)SOUTH | (uint8_t)EAST;
			}
		}
		if(w) {
			if(n) {
				h = ((uint8_t)NORTH | (uint8_t)WEST);
			}
			if(s) {
				h = ((uint8_t)SOUTH | (uint8_t)WEST);
			}
		}
		return (Direction)h;
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
