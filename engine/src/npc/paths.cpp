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

#define PATH_DEBUG
#ifdef PATH_DEBUG
#define m_debug(A) std::cerr << "[DEBUG]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";
#else
#define m_debug(A)
#endif

#define NO_INVALID_TILE_MESSAGES
#ifdef NO_INVALID_TILE_MESSAGES
#define invalid_tile(m)
#else
#define invalid_tile(_m_function) std::cerr << "[LOGIC_ERROR]: " << _m_function << " NPC on invalid tile\n";
#endif

#ifdef USE_DRAW_PATH
#define DRAW_PATH(A) draw_path(A)
#else
#define DRAW_PATH(A)
#endif

namespace wall {
	extern std::vector<Wall*> blockable_walls;
};
namespace npc::paths {
	std::array<point_t,DEMO_POINTS_SIZE> demo_points;

	bool has_line_of_sight(Actor* from,Actor* target) {
		return has_line_of_sight(get_tile(from),get_tile(target));
	}
	wall::Wall* get_tile(Actor* a) {
		if(!a) {
			return nullptr;
		}
		vpair_t p{a->rect.x,a->rect.y};
		return get_tile(p);
	}
	namespace calc {
		auto distance(int32_t x1, int32_t y1, int32_t x2,int32_t y2) {
			auto dx{x1 - x2};
			auto dy{y1 - y2};
			return std::sqrt(dx*dx + dy*dy);
		}
		template <typename TRect>
		auto distance(const TRect& src, const TRect& target) {
			auto dx{src->rect.x - target->rect.x};
			auto dy{src->rect.y - target->rect.y};
			return std::sqrt(dx*dx + dy*dy);
		}
	};
	wall::Wall* get_tile(const vpair_t& _src) {
		SDL_Rect result,src;
		src.x = _src.first;
		src.y = _src.second;
		src.w = CELL_WIDTH / 2;
		src.h = CELL_HEIGHT / 2;
		for(const auto& wall : wall::walls) {
			if(SDL_IntersectRect(&wall->rect,&src,&result)) {
				return wall.get();
			}
		}
		return nullptr;
	}
	bool has_line_of_sight(wall::Wall* from,wall::Wall* target) {
		if(!from || !target) {
			if(!from) {
				m_debug("line of sight failed: npc has invalid tile");
			}
			if(!target) {
				m_debug("line of sight failed: pc has invalid tile");
			}
			return false;
		}
		vpair_t vp_src = {from->rect.x,from->rect.y};
		vpair_t vp_target = {target->rect.x,target->rect.y};
		std::vector<vpair_t> ideal = getCoordinates(vp_src,vp_target, CELL_WIDTH);
		SDL_Rect p;
		SDL_Rect result;
		for(const auto& pair : ideal) {
			p.x = pair.first;
			p.y = pair.second;
			p.w = CELL_WIDTH / 2;
			p.h = CELL_HEIGHT / 2;
			for(const auto& wall : wall::blockable_walls) {
				if(SDL_IntersectRect(&wall->rect,&p,&result)) {
					return false;
				}
			}
		}
		return true;
	}
	std::vector<wall::Wall*> path_to_first_walkway_west(const vpair_t& src) {
		auto tile = get_tile(src);
		if(tile == nullptr) {
			invalid_tile(__FUNCTION__);
			return {};
		}
		std::vector<wall::Wall*> path;
		int32_t x = tile->rect.x - CELL_WIDTH;
		int32_t y = tile->rect.y;
		for(const auto& wall : wall::walls) {
			if(wall->rect.x == x && y == wall->rect.y) {
				path.emplace_back(wall.get());
				x -= CELL_WIDTH;
			}
		}
		return path;
	}
	std::vector<wall::Wall*> path_to_first_walkway_east(const vpair_t& src) {
		auto tile = get_tile(src);
		if(tile == nullptr) {
			invalid_tile(__FUNCTION__);
			return {};
		}
		std::vector<wall::Wall*> path;
		int32_t x = tile->rect.x + CELL_WIDTH;
		int32_t y = tile->rect.y;
		for(const auto& wall : wall::walls) {
			if(wall->rect.x == x && y == wall->rect.y) {
				path.emplace_back(wall.get());
				x += CELL_WIDTH;
			}
		}
		return path;
	}
	std::vector<wall::Wall*> path_to_first_walkway_south(const vpair_t& src) {
		auto tile = get_tile(src);
		if(tile == nullptr) {
			invalid_tile(__FUNCTION__);
			return {};
		}
		std::vector<wall::Wall*> path;
		int32_t x = tile->rect.x;
		int32_t y = tile->rect.y + CELL_HEIGHT;
		for(const auto& wall : wall::walls) {
			if(wall->rect.x == x && y == wall->rect.y) {
				path.emplace_back(wall.get());
				y += CELL_HEIGHT;
			}
		}
		return path;
	}
	std::vector<wall::Wall*> path_to_first_walkway_north(const vpair_t& src) {
		auto tile = get_tile(src);
		if(tile == nullptr) {
			invalid_tile(__FUNCTION__);
			return {};
		}
		std::vector<wall::Wall*> path;
		int32_t x = tile->rect.x;
		int32_t y = tile->rect.y - CELL_HEIGHT;
		for(const auto& wall : wall::walls) {
			if(wall->rect.x == x && y == wall->rect.y) {
				path.emplace_back(wall.get());
				y -= CELL_HEIGHT;
			}
		}
		return path;
	}

	std::vector<vpair_t> getCoordinates(const vpair_t& point1, const vpair_t& point2, int distance) {
		std::vector<vpair_t> coordinates;

		// Calculate the differences in x and y coordinates
		int dx = point2.first - point1.first;
		int dy = point2.second - point1.second;

		// Calculate the total number of steps needed
		int numSteps = std::max(std::abs(dx), std::abs(dy));
		numSteps = std::min((std::size_t)numSteps,DEMO_POINTS_SIZE);

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


	SDL_Point* PathFinder::next_point() {
		return chosen_path->next_point();
	}
	SDL_Point* ChosenPath::next_point() {
		if(traversal_ready && index < PATH_SIZE && path[index] != nullptr) {
			++index;
			if(path[index] == nullptr) {
				traversal_ready = false;
				return nullptr;
			}
			current_point = {path[index]->rect.x,path[index]->rect.y};
			return &current_point;
		}
		return nullptr;
	}
	void ChosenPath::generate(const int32_t& _src_x,
	                          const int32_t& _src_y,
	                          const int32_t& _target_x,
	                          const int32_t& _target_y) {
		src_x = _src_x;
		src_y = _src_y;
		target_x = _target_x;
		target_y = _target_y;
		update();
	}
	std::vector<wall::Wall*> ChosenPath::nearest_gateways(const int32_t& from_x,const int32_t& from_y) {
		std::map<int32_t,wall::Wall*> distance_map;
		std::vector<wall::Wall*> gw;
		for(const auto& wall : wall::walls) {
			if(wall->is_gateway == false) {
				continue;
			}
			distance_map[calc::distance(from_x,from_y,wall->rect.x,wall->rect.y)] = wall.get();
		}
		int32_t closest = 9999999;
		for(const auto& pair : distance_map) {
			if(pair.first <= closest) {
				gw.emplace_back(pair.second);
				closest = pair.first;
			}
		}
		return gw;
	}
	std::vector<wall::Wall*> ChosenPath::right_angle_path(bool& unimpeded) {
		unimpeded = true;
		vpair_t vp_src = {src_x,src_y};
		vpair_t vp_target = {target_x,target_y};

		auto heading = calculate_heading(vp_src,vp_target);
		std::vector<wall::Wall*> chosen_path;
		std::vector<wall::Wall*> first_direction;
		std::vector<wall::Wall*> second_direction;

		wall::Wall* last = nullptr;
		vpair_t vp_last;
		switch(heading) {
			case NORTH_EAST: {
					first_direction = path_to_first_walkway_east(vp_src);
					for(const auto& tile : first_direction) {
						chosen_path.emplace_back(tile);
						last = tile;
						if(tile->connections > 0) {
							break;
						}
					}
					if(!last) {
						break;
					}
					vp_last = {last->rect.x,last->rect.y};
					second_direction = path_to_first_walkway_north(vp_last);
					for(const auto& tile : second_direction) {
						chosen_path.emplace_back(tile);
						if(tile->connections > 0) {
							break;
						}
					}
					return chosen_path;
					break;
				}
			case NORTH_WEST: {
					first_direction = path_to_first_walkway_west(vp_src);
					for(const auto& tile : first_direction) {
						chosen_path.emplace_back(tile);
						last = tile;
						if(tile->connections > 0) {
							break;
						}
					}
					if(!last) {
						break;
					}
					vp_last = {last->rect.x,last->rect.y};
					second_direction = path_to_first_walkway_north(vp_last);
					for(const auto& tile : second_direction) {
						chosen_path.emplace_back(tile);
						if(tile->connections > 0) {
							break;
						}
					}
					return chosen_path;
					break;
				}
			case SOUTH_WEST: {
					first_direction = path_to_first_walkway_west(vp_src);
					for(const auto& tile : first_direction) {
						chosen_path.emplace_back(tile);
						last = tile;
						if(tile->connections > 0) {
							break;
						}
					}
					if(!last) {
						break;
					}
					vp_last = {last->rect.x,last->rect.y};
					second_direction = path_to_first_walkway_south(vp_last);
					for(const auto& tile : second_direction) {
						chosen_path.emplace_back(tile);
						if(tile->connections > 0) {
							break;
						}
					}
					return chosen_path;
					break;
				}
			case SOUTH_EAST: {
					first_direction = path_to_first_walkway_east(vp_src);
					for(const auto& tile : first_direction) {
						chosen_path.emplace_back(tile);
						last = tile;
						if(tile->connections > 0) {
							break;
						}
					}
					if(!last) {
						break;
					}
					vp_last = {last->rect.x,last->rect.y};
					second_direction = path_to_first_walkway_south(vp_last);
					for(const auto& tile : second_direction) {
						chosen_path.emplace_back(tile);
						if(tile->connections > 0) {
							break;
						}
					}
					return chosen_path;
					break;
				}
			default:
				break;
		}
		return chosen_path;
	}
	std::vector<wall::Wall*> ChosenPath::direct_path() {
		return direct_path_from(src_x,src_y,target_x,target_y);
	}
	std::vector<wall::Wall*> ChosenPath::direct_path_from(const int32_t& from_x,
	                                                      const int32_t& from_y) {
		return direct_path_from(from_x,from_y,target_x,target_y);
	}
	std::vector<wall::Wall*> ChosenPath::direct_path_from(const int32_t& from_x,
	                                                      const int32_t& from_y,
	                                                      const int32_t& to_x,
	                                                      const int32_t& to_y) {
		direct_path_unimpeded = true;
		vpair_t vp_src = {from_x,from_y};
		vpair_t vp_target = {to_x,to_y};
		std::vector<vpair_t> ideal = getCoordinates(vp_src,vp_target, CELL_WIDTH);
		SDL_Rect p;
		SDL_Rect result;
		std::vector<wall::Wall*> chosen_path;
		for(const auto& pair : ideal) {
			p.x = pair.first;
			p.y = pair.second;
			p.w = CELL_WIDTH / 2;
			p.h = CELL_HEIGHT / 2;
			for(const auto& wall : wall::blockable_walls) {
				if(SDL_IntersectRect(&wall->rect,&p,&result)) {
					obstacles.emplace_back(wall);
					direct_path_unimpeded = false;
				}
			}
			for(const auto& wall : wall::walkable_walls) {
				if(SDL_IntersectRect(&wall->rect,&p,&result)) {
					chosen_path.emplace_back(wall);
				}
			}
		}
		return chosen_path;
	}
	std::vector<wall::Wall*> ChosenPath::direct_path_from(wall::Wall* tile) {
		return direct_path_from(tile->rect.x,tile->rect.y);
	}
	void ChosenPath::save_path(std::initializer_list<std::vector<wall::Wall*>> l) {
		std::fill(path.begin(),path.end(),nullptr);
		path_elements = 0;
		index = 0;
		std::size_t i = 0;
		for(const auto& vec : l) {
			for(const auto& tile : vec) {
				path[i] = tile;
				++i;
				if(i >= PATH_SIZE) {
					break;
				}
			}
			if(i >= PATH_SIZE) {
				break;
			}
		}
		if(i >= PATH_SIZE) {
			i = PATH_SIZE - 1;
		}
		auto first = path[0];
		for(std::size_t x=1; x < PATH_SIZE && first == path[x] && path[x] != nullptr; ++x) {
			index = x;
			//std::cerr << "iterate, catchup: " << index << "\n";
		}

		path_elements = i;
		path[i] = nullptr;
		traversal_ready = true;
	}
	void ChosenPath::save_path(const std::vector<wall::Wall*>& in_path) {
		return save_path({in_path});
	}
	bool ChosenPath::attempt_gateway_method() {
		uint8_t state = 0;
		std::vector<wall::Wall*> path;
		auto gateways = nearest_gateways(src_x,src_y);
		bool found_direct_path_to_gw = false;
		for(const auto& tile : gateways) {
			auto line_to_gw = direct_path_from(tile->rect.x,tile->rect.y);
			if(direct_path_unimpeded) {
				found_direct_path_to_gw = true;
				path.insert(path.end(),line_to_gw.cbegin(),line_to_gw.cend());
				++state;
				break;
			}
		}
		if(found_direct_path_to_gw) {
			auto tmp = direct_path_from(path.back());
			if(direct_path_unimpeded) {
				save_path({path,tmp});
				++state;
				m_debug("gw + direct (" << state << "/2)");
				return true;
			}
			m_debug(state << "/2");
			return false;
		}
		m_debug(state << "/2");

		return false;
	}
	bool ChosenPath::attempt_direct_path() {
		auto path = direct_path();
		if(direct_path_unimpeded) {
			m_debug("Path unimpeded for direct");
			DRAW_PATH(path);
			save_path(path);
			return true;
		}
		return false;
	}
	bool ChosenPath::attempt_right_angle() {
		bool right_angle_unimpeded = false;
		auto path = right_angle_path(right_angle_unimpeded);
		if(path.size()) {
			if(right_angle_unimpeded) {
				m_debug("Path unimpeded for right angle 1");
				DRAW_PATH(path);
				save_path(path);
				return true;
			}
			auto direct = direct_path_from(path.back());
			if(direct_path_unimpeded) {
				m_debug("Path unimpeded for right angle with direct path after");
				DRAW_PATH(path);
				save_path({path,direct});
				return true;
			}
		}
		return false;
	}
	void ChosenPath::update() {
		//if(attempt_direct_path()) {
		//	m_debug("direct path sufficed");
		//	return;
		//}
		if(attempt_right_angle()) {
			m_debug("right angle sufficed");
			return;
		}
		//if(attempt_gateway_method()) {
		//	m_debug("gateway method sufficed");
		//	return;
		//}
		traversal_ready = false;

	}
	void ChosenPath::update(const Actor* src,const Actor* targ) {
		if(src != nullptr) {
			src_x = src->rect.x;
			src_y = src->rect.y;
		}
		if(targ != nullptr) {
			target_x = targ->rect.x;
			target_y = targ->rect.y;
		}
		update();
	}
	ChosenPath::ChosenPath(const int32_t& _src_x,
	                       const int32_t& _src_y,
	                       const int32_t& _target_x,
	                       const int32_t& _target_y) {
		traversal_ready = false;
		path_elements = 0;
		std::fill(path.begin(),path.end(),nullptr);
		index = 0;
		direct_path_tried = false;
		//generate(_src_x,_src_y,_target_x,_target_y);
	}
	void PathFinder::update(Actor* _in_hunter,Actor* _in_target) {
		if(!chosen_path) {
			chosen_path = std::make_unique<ChosenPath>(_in_hunter->rect.x,
			                                           _in_hunter->rect.y,
			                                           _in_target->rect.x,
			                                           _in_target->rect.y);
		}
		chosen_path->update(_in_hunter,_in_target);

		m_hunter = _in_hunter;
		m_target = _in_target;
		x = _in_hunter->rect.x;
		y = _in_hunter->rect.y;
		target_x = _in_target->rect.x;
		target_y = _in_target->rect.y;

		start_point = {x,y};
		destination_point = {target_x,target_y};
		heading = calculate_heading();
	}
	/**
	 * Definitions:
	 * Gateway: how to get past obstacles
	 * Walkway: connects gateways to each other
	*
	* A naive implementation of a path-finding algorithm:
	* - get heading of target
	* - split heading into two directions
	*   - for example, south + east
	* - find nearest walkway
	* - walk to nearest walkway, until arrived at nearest walkway
	 *
	 */
	std::map<int32_t,wall::Wall*> PathFinder::get_closest_gateways(Actor* _from) {
		std::map<int32_t,wall::Wall*> closest_gateways;
		if(_from == nullptr) {
			_from = m_hunter;
		}
		int32_t closest = 99999;
		for(const auto& gw : wall::gateways) {
			int32_t dist = gw->distance_to(&_from->rect);
			if(dist <= closest) {
				closest_gateways[dist] = gw;
				closest = dist;
			}
		}
		return closest_gateways;
	}

	PathFinder::PathFinder(int hunter_movement_amount, Actor* _in_hunter,Actor* _in_target)
		:
		movement_amount(hunter_movement_amount),
		x(_in_hunter->rect.x),
		y(_in_hunter->rect.y),
		target_x(_in_target->rect.x),
		target_y(_in_target->rect.y) {
		m_hunter = _in_hunter;
		m_target = _in_target;
	}
	bool PathFinder::is_obstacle(const SDL_Point* _p) {
		return std::find(obstacles.cbegin(),obstacles.cend(),_p) != obstacles.cend();
	}
	Direction PathFinder::calculate_heading() {
		return npc::paths::calculate_heading(x,y,target_x,target_y);
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

#undef m_debug
#undef invalid_tile
#undef DRAW_PATH
