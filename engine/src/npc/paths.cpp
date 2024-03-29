#include <SDL2/SDL.h>
#include <iostream>
#define USE_PATH_TESTING_NORTH_EAST
#include "paths.hpp"
#include "../player.hpp"
#include "../direction.hpp"
#include "../draw.hpp"
#include "../wall.hpp"
#include "../movement.hpp"
#include "../time.hpp"
#include <cmath>
#include <tuple>
#include <map>
#include <array>
#include <algorithm>
#include <set>
// FIXME
//#define SHOW_HELPFUL_GRAPH_STUFF
//#define SHOW_PATHFINDER_ANIMATE
//#define PATH_DEBUG

#ifdef SHOW_HELPFUL_GRAPH_STUFF
#define USE_DRAW_PATH 1
#endif

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

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[NPC][PATHS][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[NPC][PATHS][ERROR]: " << A << "\n";

namespace wall {
	extern std::vector<Wall*> blockable_walls;
	extern std::vector<Wall*> walkable_walls;
};
struct from_to_ptr {
  wall::Wall* from;
  wall::Wall* to;
  bool value;
};
struct from_to_index_t {
  uint16_t* from;
  uint16_t* to;
  bool value;
};
namespace npc::paths {
	std::array<point_t,DEMO_POINTS_SIZE> demo_points;
	std::array<point_t,DEMO_POINTS_SIZE> gw_points;
  static std::vector<uint32_t> index_connects;
  static std::vector<uint32_t> index_blocks;
  static size_t los_call_count = 0;
  static size_t los_load = 0;
  static size_t los_cache_hit = 0;
  int get_direction_toward_player(SDL_Rect* r){
    bool plr_is_east = false;
    bool plr_is_north = false;
    bool plr_is_west = false;
    bool plr_is_south = false;
    if(r->x <= plr::self()->rect.x){
      plr_is_east = true;
    }
    if(r->x >= plr::self()->rect.x){
      plr_is_west = true;
    }
    if(r->y <= plr::self()->rect.y){
      plr_is_south = true;
    }
    if(r->y >= plr::self()->rect.y){
      plr_is_north = true;
    }
    if(plr_is_east && plr_is_north){
      return NORTH_EAST;
    }
    if(plr_is_east && plr_is_south){
      return SOUTH_EAST;
    }
    if(plr_is_east && !plr_is_north && !plr_is_south){
      return EAST;
    }
    if(plr_is_west && !plr_is_north && !plr_is_south){
      return WEST;
    }
    if(plr_is_west && plr_is_north){
      return NORTH_WEST;
    }
    if(plr_is_west && plr_is_south){
      return SOUTH_WEST;
    }
    m_error("Shouldn't get here");
    return NORTH;
  }
  void report(){
    std::cout << "los_call_count: " << los_call_count << "\n";
    std::cout << "los_cache_hit: " << los_cache_hit << "\n";
    std::cout << "los_load: " << los_load << "\n";
    std::cout << "index_connects: " << index_connects.size() << "\n";
    std::cout << "index_blocks: " << index_blocks.size() << "\n";
    std::cout << xorlock_time::now_string() << "\n";
  }

	wall::Wall* get_tile(const int32_t& x,const int32_t& y) {
		SDL_Rect result,src;
		src.x = x;
		src.y = y;
		src.w = CELL_WIDTH / 2;
		src.h = CELL_HEIGHT / 2;
		for(const auto& wall : wall::walls) {
			if(SDL_IntersectRect(&wall->self.rect,&src,&result)) {
				return wall.get();
			}
		}
		return nullptr;
	}
	wall::Wall* get_tile(vpair_t _src) {
		SDL_Rect result,src;
		src.x = _src.first;
		src.y = _src.second;
		src.w = CELL_WIDTH / 2;
		src.h = CELL_HEIGHT / 2;
		for(const auto& wall : wall::walls) {
			if(SDL_IntersectRect(&wall->self.rect,&src,&result)) {
				return wall.get();
			}
		}
		return nullptr;
	}
	wall::Wall* get_tile(const vpair_t& _src) {
		SDL_Rect result,src;
		src.x = _src.first;
		src.y = _src.second;
		src.w = CELL_WIDTH / 2;
		src.h = CELL_HEIGHT / 2;
		for(const auto& wall : wall::walls) {
			if(SDL_IntersectRect(&wall->self.rect,&src,&result)) {
				return wall.get();
			}
		}
		return nullptr;
	}
	wall::Wall* get_tile(Actor* a) {
		return get_tile(a->rect.x,a->rect.y);
	}
  bool has_line_of_sight(SDL_Rect src, SDL_Rect target){
    auto from = get_tile(src.x,src.y);
    auto targ = get_tile(target.x,target.y);
    return has_line_of_sight(from,targ);
  }
  bool has_line_of_sight(wall::Wall* from,wall::Wall* target) {
    ++los_call_count;
    if(!from){
      return false;
    }
    uint32_t m = from->index;
    m <<= 16;
    m |= target->index;
    auto it_connects = std::find(index_connects.cbegin(),index_connects.cend(),m);
    if(it_connects != index_connects.cend()){
      ++los_cache_hit;
      return true;
    }
    auto it_blocks = std::find(index_blocks.cbegin(),index_blocks.cend(),m);
    if(it_blocks != index_blocks.cend()){
      ++los_cache_hit;
      return false;
    }

    vpair_t vp_src,vp_target;
    bool blocked = false;
    std::vector<vpair_t> ideal;
    SDL_Rect p;
    SDL_Rect result;
    vp_src = {from->self.rect.x,from->self.rect.y};
    vp_target = {target->self.rect.x,target->self.rect.y};
    ideal = getCoordinates(vp_src,vp_target, CELL_WIDTH);
    blocked = false;
    for(auto&& pair : ideal) {
      p.x = pair.first;
      p.y = pair.second;
      p.w = CELL_WIDTH / 2;
      p.h = CELL_HEIGHT / 2;
      for(const auto& wall : wall::blockable_walls) {
        if(SDL_IntersectRect(&wall->self.rect,&p,&result)) {
          ++los_load;
          index_blocks.emplace_back(m);
          return false;
        }
      }
    }
    if(!blocked){
      ++los_load;
      index_connects.emplace_back(m);
    }
    return !blocked;
  }
  void load_los_cache(){
    m_debug("TODO FIXME");
    index_connects.reserve(2000);
    index_blocks.reserve(2000);
  }
	uint16_t longest_west_walkway(std::vector<wall::Wall*>* storage,const int32_t& x,const int32_t& y,
	                              const int32_t& dest_x,
	                              const int32_t& dest_y) {
		uint16_t ctr = 0;
		auto d_tile = get_tile(dest_x,dest_y);
		wall::Wall* node = get_tile(x,y);
		if(!d_tile || !node) {
			return 0;
		}
		for(; node != nullptr; node = node->west) {
			if(!node->walkable) {
				return ctr;
			}
			storage->emplace_back(node);
			++ctr;
			if(node->self.rect.x == d_tile->self.rect.x) {
				// we've gone too far west
				return ctr;
			}
		}
		return ctr;
	}
	uint16_t longest_east_walkway(std::vector<wall::Wall*>* storage,const int32_t& x,const int32_t& y,
	                              const int32_t& dest_x,
	                              const int32_t& dest_y) {
		uint16_t ctr = 0;
		auto d_tile = get_tile(dest_x,dest_y);
		wall::Wall* node = get_tile(x,y);
		if(!d_tile || !node) {
			return 0;
		}
		for(; node != nullptr; node = node->east) {
			if(!node->walkable) {
				return ctr;
			}
			storage->emplace_back(node);
			++ctr;
			if(node->self.rect.x == d_tile->self.rect.x) {
				// we've gone too far east
				return ctr;
			}
		}
		return ctr;
	}
	uint16_t longest_south_walkway(std::vector<wall::Wall*>* storage,const int32_t& x,const int32_t& y,
	                               const int32_t& dest_x,
	                               const int32_t& dest_y) {
		uint16_t ctr = 0;
		auto d_tile = get_tile(dest_x,dest_y);
		wall::Wall* node = get_tile(x,y);
		if(!d_tile || !node) {
			return 0;
		}
		for(; node != nullptr; node = node->south) {
			if(!node->walkable) {
				return ctr;
			}
			storage->emplace_back(node);
			++ctr;
			if(node->self.rect.y == d_tile->self.rect.y) {
				// we've gone too far south
				return ctr;
			}
		}
		return ctr;
	}
	uint16_t longest_north_walkway(std::vector<wall::Wall*>* storage,const int32_t& x,const int32_t& y,
	                               const int32_t& dest_x,
	                               const int32_t& dest_y) {
		uint16_t ctr = 0;
		auto d_tile = get_tile(dest_x,dest_y);
		wall::Wall* node =  get_tile(x,y);
		if(!d_tile || !node) {
			return 0;
		}
		for(; node != nullptr; node = node->north) {
			if(!node->walkable) {
				return ctr;
			}
			storage->emplace_back(node);
			++ctr;
			if(node->self.rect.y == d_tile->self.rect.y) {
				// we've gone too far north
				return ctr;
			}
		}
		return ctr;
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
			if(npc::paths::distance(coordinate.first,coordinate.second,point2.first,point2.second) <= CELL_WIDTH) {
				return coordinates;
			}
		}

		return coordinates;
	}


	SDL_Point* PathFinder::next_point() {
		return chosen_path->next_point();
	}
	bool ChosenPath::target_on_tile(wall::Wall* tile) const {
		auto target_tile = get_tile(target_x,target_y);
		return target_tile == tile;
	}
	SDL_Point* ChosenPath::next_point() {
		if(traversal_ready && index < PATH_SIZE && path[index] != nullptr) {
			++index;
			if(path[index] == nullptr) {
				traversal_ready = false;
				return nullptr;
			}
			current_point = {path[index]->self.rect.x,path[index]->self.rect.y};
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
		if(attempt_direct_path() || direct_line_only) {
      return;
    }
		if(attempt_right_angle()) {
      return;
    }
    traversal_ready = true;
	}
	std::vector<wall::Wall*> ChosenPath::nearest_gateways(const int32_t& from_x,const int32_t& from_y) {
		std::map<int32_t,wall::Wall*> distance_map;
		std::vector<wall::Wall*> gw;
		for(const auto& wall : wall::walls) {
			if(wall->is_gateway == false) {
				continue;
			}
			distance_map[distance(from_x,from_y,wall->self.rect.x,wall->self.rect.y)] = wall.get();
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
	bool ChosenPath::target_close_to_tile(wall::Wall* tile) const {
		auto dis = distance(tile->self.rect.x, tile->self.rect.y, target_x,target_y);
		return dis <= CELL_WIDTH * 2;
	}
	Direction ChosenPath::calculate_heading() {
		Direction h = NORTH;
		bool n,s,e,w;
		n = s = e = w = 0;

		if(src_x <= target_x) {
			e = true;
			h = EAST;
		} else {
			w = true;
			h = WEST;
		}
		if(src_y <= target_y) {
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
	bool ChosenPath::has_line_of_sight_from(wall::Wall* tile) {
		vpair_t vp_src = {tile->self.rect.x,tile->self.rect.y};
		vpair_t vp_target = {target_x,target_y};
		std::vector<vpair_t> ideal = getCoordinates(vp_src,vp_target, CELL_WIDTH);
		SDL_Rect p;
		SDL_Rect result;
		for(const auto& pair : ideal) {
			p.x = pair.first;
			p.y = pair.second;
			p.w = CELL_WIDTH / 2;
			p.h = CELL_HEIGHT / 2;
			for(const auto& wall : wall::blockable_walls) {
				if(SDL_IntersectRect(&wall->self.rect,&p,&result)) {
					return false;
				}
			}
		}
		return true;
	}

	uint16_t ChosenPath::direct_path_from(std::vector<wall::Wall*> * storage) {
		uint16_t ctr = 0;
		if(storage->size() == 0) {
			return 0;
		}
		const auto& tile = storage->back();
		auto dst_tile = get_tile(target_x,target_y);
		vpair_t src{tile->self.rect.x,tile->self.rect.y};
		vpair_t dst{dst_tile->self.rect.x,dst_tile->self.rect.y};
		std::vector<vpair_t> ideal = getCoordinates(src,dst, CELL_WIDTH);
    std::unordered_map<uint16_t,uint16_t> block_count;
		for(const auto& pair : ideal) {
			auto next_tile = get_tile(pair.first,pair.second);
      if(block_count[next_tile->block]++ < 2){
			  storage->emplace_back(next_tile);
      }
			++ctr;
		}
		return ctr;

	}
	std::vector<wall::Wall*> ChosenPath::right_angle_path(bool& unimpeded,bool swap) {
		unimpeded = false;

		std::vector<wall::Wall*> chosen_path;
		uint16_t ctr = 0;
		switch(calculate_heading()) {
			case NORTH_EAST: {
					if(swap) {
						ctr = longest_east_walkway(&chosen_path,
						                           src_x,src_y,
						                           target_x, target_y);
						if(ctr) {
							ctr = longest_north_walkway(&chosen_path,
							                            chosen_path.back()->self.rect.x,
							                            chosen_path.back()->self.rect.y,
							                            target_x,target_y);
							if(ctr && has_line_of_sight_from(chosen_path.back())) {
								direct_path_from(&chosen_path);
								unimpeded = true;
								return chosen_path;
							}
						}
					} else {
						ctr = longest_north_walkway(&chosen_path,
						                            src_x,src_y,
						                            target_x, target_y);
						if(ctr) {
							ctr = longest_east_walkway(&chosen_path,
							                           chosen_path.back()->self.rect.x,
							                           chosen_path.back()->self.rect.y,
							                           target_x,target_y);
							if(ctr && has_line_of_sight_from(chosen_path.back())) {
								direct_path_from(&chosen_path);
								unimpeded = true;
								return chosen_path;
							}
						}
					}
					break;
				}
			case SOUTH_WEST: {
					if(swap) {
						chosen_path.clear();
						ctr = longest_west_walkway(&chosen_path,
						                           src_x,src_y,
						                           target_x, target_y);
						if(ctr) {
							ctr = longest_south_walkway(&chosen_path,
							                            chosen_path.back()->self.rect.x,
							                            chosen_path.back()->self.rect.y,
							                            target_x,target_y);
							if(ctr && has_line_of_sight_from(chosen_path.back())) {
								direct_path_from(&chosen_path);
								unimpeded = true;
							}
						}
						return chosen_path;
					} else {
						ctr = longest_south_walkway(&chosen_path,
						                            src_x,src_y,
						                            target_x, target_y);
						if(ctr) {
							ctr = longest_west_walkway(&chosen_path,
							                           chosen_path.back()->self.rect.x,
							                           chosen_path.back()->self.rect.y,
							                           target_x,target_y);
							if(ctr && has_line_of_sight_from(chosen_path.back())) {
								direct_path_from(&chosen_path);
								unimpeded = true;
							}
						}
						return chosen_path;
					}
					break;
				}
			case SOUTH_EAST: {
					if(swap) {
						ctr = longest_east_walkway(&chosen_path,
						                           src_x,src_y,
						                           target_x, target_y);
						if(ctr) {
							ctr = longest_south_walkway(&chosen_path,
							                            chosen_path.back()->self.rect.x,
							                            chosen_path.back()->self.rect.y,
							                            target_x,target_y);
							if(ctr && has_line_of_sight_from(chosen_path.back())) {
								direct_path_from(&chosen_path);
								unimpeded = true;
								return chosen_path;
							}
						}
					} else {
						ctr = longest_south_walkway(&chosen_path,
						                            src_x,src_y,
						                            target_x, target_y);
						if(ctr) {
							ctr = longest_east_walkway(&chosen_path,
							                           chosen_path.back()->self.rect.x,
							                           chosen_path.back()->self.rect.y,
							                           target_x,target_y);
							if(ctr && has_line_of_sight_from(chosen_path.back())) {
								direct_path_from(&chosen_path);
								unimpeded = true;
								return chosen_path;
							}
						}
					}
					break;
				}
			case NORTH_WEST: {
					if(swap) {
						ctr = longest_west_walkway(&chosen_path,
						                           src_x,src_y,
						                           target_x, target_y);
						if(ctr) {
							ctr = longest_north_walkway(&chosen_path,
							                            chosen_path.back()->self.rect.x,
							                            chosen_path.back()->self.rect.y,
							                            target_x,target_y);
							if(ctr && has_line_of_sight_from(chosen_path.back())) {
								direct_path_from(&chosen_path);
								unimpeded = true;
								return chosen_path;
							}
						}
					} else {
						ctr = longest_north_walkway(&chosen_path,
						                            src_x,src_y,
						                            target_x, target_y);
						if(ctr) {
							ctr = longest_west_walkway(&chosen_path,
							                           chosen_path.back()->self.rect.x,
							                           chosen_path.back()->self.rect.y,
							                           target_x,target_y);
							if(ctr && has_line_of_sight_from(chosen_path.back())) {
								direct_path_from(&chosen_path);
								unimpeded = true;
								return chosen_path;
							}
						}
					}
					break;
				}
			default:
				break;
		}
		return chosen_path;
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
        if(SDL_IntersectRect(&wall->self.rect,&p,&result)) {
          obstacles.emplace_back(wall);
          direct_path_unimpeded = false;
        }
      }
      for(const auto& wall : wall::walkable_walls) {
        if(SDL_IntersectRect(&wall->self.rect,&p,&result)) {
          chosen_path.emplace_back(wall);
        }
      }
    }
    return chosen_path;
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
      auto line_to_gw = direct_path_from(tile->self.rect.x,tile->self.rect.y,target_x,target_y);
      if(direct_path_unimpeded) {
        found_direct_path_to_gw = true;
        path.insert(path.end(),line_to_gw.cbegin(),line_to_gw.cend());
        ++state;
        break;
      }
    }
    if(found_direct_path_to_gw) {
      auto tmp = direct_path_from(path.back()->self.rect.x,path.back()->self.rect.y,target_x,target_y);
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
    auto path = direct_path_from(src_x,src_y,target_x,target_y);
    if(direct_path_unimpeded) {
      m_debug("Path unimpeded for direct");
      DRAW_PATH(path);
      save_path(path);
      return true;
    }
    return false;
  }
  template <typename T>
    bool close_enough(const T& a,const T& b) {
      return a - b < (CELL_WIDTH * 4);
    }
  void clear_gw() {
    for(auto& g : gw_points) {
      g = {{0,0},false};
    }
  }
  template <typename T>
    void feed_gw(const T& container) {
      std::size_t i = 0;
      for(const auto& tile : container) {
        if(tile == nullptr) {
          continue;
        }
        gw_points[i++] = {{tile->self.rect.x,tile->self.rect.y},true};
      }
    }
  void ChosenPath::populate_nearest_target_gateways() {
    std::fill(nearest_target_gateways.begin(),nearest_target_gateways.end(),nullptr);
    float closest = (CELL_WIDTH * 8);
    std::size_t ntg_index = 0;
    float dist = 0;
    std::set<wall::Wall*> set;
    for(const auto& wall : wall::walls) {
      if(wall->is_gateway) {
        dist = distance(target_x,target_y,wall->self.rect.x,wall->self.rect.y);
        if(dist < closest) {
          if(set.find(wall.get()) == set.end()) {
            set.insert(wall.get());
            nearest_target_gateways[ntg_index++] = wall.get();
            if(ntg_index >= NTG_SIZE) {
              m_debug("ntg_index max reached");
              break;
            }
          }
        }
      }
    }
    closest = (CELL_WIDTH * 8);
    std::forward_list<wall::Wall*> best;
    for(ntg_index = 0; ntg_index < NTG_SIZE; ntg_index++) {
      if(nearest_target_gateways[ntg_index] == nullptr) {
        continue;
      }
      const auto& wall = nearest_target_gateways[ntg_index];
      dist = distance(target_x,target_y,wall->self.rect.x,wall->self.rect.y);
      if(dist < closest) {
        best.push_front(wall);
        closest = dist;
      }
    }
    std::fill(nearest_target_gateways.begin(),nearest_target_gateways.end(),nullptr);
    ntg_index = 0;
    for(const auto& tile : best) {
      nearest_target_gateways[ntg_index++] = tile;
      if(ntg_index >= NTG_SIZE) {
        break;
      }
    }
  }
  bool ChosenPath::attempt_reverse_gateway() {
    /**
     * Get the nearest gateway to the target
     */
    populate_nearest_target_gateways();
    clear_gw();
    feed_gw(nearest_target_gateways);
    return true;
  }
  bool ChosenPath::attempt_right_angle() {
    bool right_angle_unimpeded = false;
    auto path = right_angle_path(right_angle_unimpeded,false);
    if(!right_angle_unimpeded) {
      path = right_angle_path(right_angle_unimpeded,true);
    }
    if(path.size()) {
      if(right_angle_unimpeded) {
        m_debug("Path unimpeded for right angle 1");
        DRAW_PATH(path);
        save_path(path);
        return true;
      }
      auto direct = direct_path_from(path.back()->self.rect.x,path.back()->self.rect.y,target_x,target_y);
      if(direct_path_unimpeded) {
        m_debug("Path unimpeded for right angle with direct path after");
        DRAW_PATH(path);
        save_path({path,direct});
        return true;
      }
    }
    return false;
  }

  void ChosenPath::update(const Actor* src,const Actor* targ) {
#ifdef USE_SAFE_CHOSEN_PATH_CHECKS
    if(src != nullptr) {
      src_x = src->rect.x;
      src_y = src->rect.y;
    } else {
      m_debug("update failed. src is nullptr");
    }
    if(targ != nullptr) {
      target_x = targ->rect.x;
      target_y = targ->rect.y;
    } else {
      m_debug("update failed. target is nullptr");
    }
#else
    src_x = src->rect.x;
    src_y = src->rect.y;
    target_x = targ->rect.x;
    target_y = targ->rect.y;
    if(attempt_direct_path() || direct_line_only) {
      return;
    }
    if(attempt_right_angle()) {
      return;
    }
    traversal_ready = true;
#endif
  }
  void PathFinder::set_direct_line_only(bool v){
    m_direct_line_only = v;
    chosen_path->direct_line_only = v;
  }
  ChosenPath::ChosenPath(){
    direct_line_only = false;
    traversal_ready = false;
    path_elements = 0;
    std::fill(path.begin(),path.end(),nullptr);
    index = 0;
    direct_path_tried = false;
    //generate(_src_x,_src_y,_target_x,_target_y);
  }
  void PathFinder::update(Actor* _in_hunter,Actor* _in_target) {
    chosen_path->update(_in_hunter,_in_target);

    m_hunter = _in_hunter;
    m_target = _in_target;
    x = _in_hunter->rect.x;
    y = _in_hunter->rect.y;
    target_x = _in_target->rect.x;
    target_y = _in_target->rect.y;

    start_point = {x,y};
    destination_point = {target_x,target_y};
    heading = chosen_path->calculate_heading();
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

      chosen_path = std::make_unique<ChosenPath>();
      }
  bool PathFinder::is_obstacle(const SDL_Point* _p) {
    return std::find(obstacles.cbegin(),obstacles.cend(),_p) != obstacles.cend();
  }
  PathFinder::~PathFinder(){
    chosen_path = nullptr;
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
