#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "movement.hpp"
#include "player.hpp"
#include "world.hpp"
#include "map.hpp"
#include "wall.hpp"
#include "weapons/grenade.hpp"
#include <sys/time.h>

#ifdef USE_OMP
// TODO: guard with macros in case target platform doesnt have libomp
#include <omp.h>
#endif

#ifdef m_debug
#undef m_debug
#endif
#define m_debug(A) std::cerr << "[DEBUG][movement.cpp]:" << A << "\n";

std::vector<wall::Wall*> near_walls(SDL_Rect* actor_rect) {
	std::vector<wall::Wall*> nearest;
	SDL_Rect bubble = *actor_rect;
	bubble.w += 80;
	bubble.h += 80;
	bubble.x -= 40;
	bubble.y -= 40;
	for(const auto& wall : wall::blockable_walls) {
		if(SDL_TRUE == SDL_HasIntersection(&bubble,&wall->self.rect)) {
			nearest.emplace_back(wall);
		}
	}
	return nearest;
}

bool can_move_direction(int direction,SDL_Rect* p,int adjustment) {
	SDL_Rect result;
	SDL_Rect north_of = *p;
	north_of.y -= adjustment;

	SDL_Rect north_east_of = *p;
	north_east_of.y -= adjustment;
	north_east_of.x += adjustment;

	SDL_Rect north_west_of = *p;
	north_west_of.y -= adjustment;
	north_west_of.x -= adjustment;

	SDL_Rect south_of = *p;
	south_of.y += adjustment;

	SDL_Rect south_west_of = *p;
	south_west_of.y += adjustment;
	south_west_of.x -= adjustment;

	SDL_Rect south_east_of = *p;
	south_east_of.y += adjustment;
	south_east_of.x += adjustment;

	SDL_Rect east_of = *p;
	east_of.x += adjustment;
	SDL_Rect west_of = *p;
	west_of.x -= adjustment;
	SDL_Rect* ptr = nullptr;
	switch(direction) {
		case NORTH_EAST:
			ptr = &north_east_of;
			break;
		case NORTH_WEST:
			ptr = &north_west_of;
			break;
		case NORTH:
			ptr = &north_of;
			break;
		case EAST:
			ptr = &east_of;
			break;
		case SOUTH_EAST:
			ptr = &south_east_of;
			break;
		case SOUTH_WEST:
			ptr = &south_west_of;
			break;
		case SOUTH:
			ptr = &south_of;
			break;
		case WEST:
			ptr = &west_of;
			break;
		default:
			break;
	}

	for(const auto& wall : near_walls(p)) {
		if(SDL_TRUE == SDL_HasIntersection(ptr,&wall->self.rect)) {
			return false;
		}
	}
	return true;
}
std::pair<bool,uint8_t> check_can_move(SDL_Rect* p, int dir, int amount) {
	SDL_Rect base{p->x,p->y,p->w,p->h};
	if(dir == NORTH) {
		return {can_move_direction(NORTH,&base,amount),(Direction)dir};
	}
	if(dir == SOUTH) {
		return {can_move_direction(SOUTH,&base,amount),(Direction)dir};
	}
	if(dir == EAST) {
		return {can_move_direction(EAST,&base,amount),(Direction)dir};
	}
	if(dir == WEST) {
		return {can_move_direction(WEST,&base,amount),(Direction)dir};
	}
	if(dir == NORTH_WEST) {
		bool north = can_move_direction(NORTH,&base,amount);
		bool west = can_move_direction(WEST,&base,amount);
		if(north && west) {
			return {true,NORTH_WEST};
		}
		if(north) {
			return {true,NORTH};
		}
		if(west) {
			return {true,WEST};
		}
	}
	if(dir == NORTH_EAST) {
		bool north= can_move_direction(NORTH,&base,amount);
		bool east = can_move_direction(EAST,&base,amount);
		if(north && east) {
			return {true,NORTH_EAST};
		}
		if(north) {
			return {true,NORTH};
		}
		if(east) {
			return {true,EAST};
		}
	}
	if(dir == SOUTH_WEST) {
		bool south = can_move_direction(SOUTH,&base,amount);
		bool west = can_move_direction(WEST,&base,amount);
		if(south && west) {
			return {true,SOUTH_WEST};
		}
		if(south) {
			return {true,SOUTH};
		}
		if(west) {
			return {true,WEST};
		}
	}
	if(dir == SOUTH_EAST) {
		bool south = can_move_direction(SOUTH,&base,amount);
		bool east = can_move_direction(EAST,&base,amount);
		if(south && east) {
			return {true,SOUTH_EAST};
		}
		if(south) {
			return {true,SOUTH};
		}
		if(east) {
			return {true,EAST};
		}
	}
	return {false,NORTH};
}

std::pair<bool,uint8_t> MovementManager::can_move(int direction,int amount) {
	return check_can_move(plr::get_effective_move_rect(),direction, plr::movement_amount() + (plr::movement_amount() * 1.03));
}

void MovementManager::move_map(Direction dir,int amount) {
	int adjustment = 0;
	amount *= 2;
	switch(dir) {
		case NORTH:
			world->y -= amount;
			adjustment = - 1 * amount;
			if(plr::cy() >= win_height() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			plr::self()->world_y -= amount;
			break;
		case NORTH_WEST:
			world->x -= amount;
			world->y -= amount;
			adjustment = - 1 * amount;
			if(plr::cx() <= win_width() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			plr::self()->world_x -= amount;
			plr::self()->world_y -= amount;
			break;
		case NORTH_EAST:
			world->x += amount;
			world->y -= amount;
			adjustment = abs(amount);
			adjustment = - 1 * amount;
			if(plr::cx() <= win_width() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			plr::self()->world_x += amount;
			plr::self()->world_y -= amount;
			break;
		case SOUTH:
			world->y += amount;
			adjustment = abs(amount);
			if(plr::cy() <= win_height() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			plr::self()->world_y += amount;
			break;
		case SOUTH_EAST:
			world->y += amount;
			world->x += amount;
			adjustment = abs(amount);
			if(plr::cy() <= win_height() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			plr::self()->world_x += amount;
			plr::self()->world_y += amount;
			break;
		case SOUTH_WEST:
			world->y += amount;
			world->x -= amount;
			adjustment = abs(amount);
			if(plr::cy() <= win_height() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			plr::self()->world_x -= amount;
			plr::self()->world_y += amount;
			break;
		case WEST:
			world->x -= amount;
			adjustment = - 1 * amount;
			if(plr::cx() >= win_width() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			plr::self()->world_x -= amount;
			break;
		case EAST:
			world->x += amount;
			adjustment = abs(amount);
			if(plr::cx() <= win_width() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			plr::self()->world_x += amount;
			break;
		default:
			break;
	}
  auto& actors = world->actors();
  std::cout << "actors: " << actors.size() << "\n";
  
  struct timeval tv;
  gettimeofday(&tv,NULL);
  unsigned long time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

#ifdef USE_OMP
  #pragma omp parallel for
#endif
  for(int i=0; i < actors.size();i++){
		switch(dir) {
			case NORTH_EAST:
				actors[i]->rect.y += amount;
				actors[i]->rect.x -= amount;
				break;
			case NORTH_WEST:
				actors[i]->rect.y += amount;
				actors[i]->rect.x += amount;
				break;
			case NORTH:
				actors[i]->rect.y += amount;
				break;
			case SOUTH_EAST:
				actors[i]->rect.y -= amount;
				actors[i]->rect.x -= amount;
				break;
			case SOUTH_WEST:
				actors[i]->rect.y -= amount;
				actors[i]->rect.x += amount;
				break;
			case SOUTH:
				actors[i]->rect.y -= amount;
				break;
			case WEST:
				actors[i]->rect.x += amount;
				break;
			case EAST:
				actors[i]->rect.x -= amount;
				break;
			default:
				break;
		}
  }

  struct timeval after;
  gettimeofday(&after,NULL);
  unsigned long after_time_in_micros = 1000000 * after.tv_sec + after.tv_usec;
  std::cout << "time: " << after_time_in_micros - time_in_micros << "\n";

  for(auto& pair : world->points()){
		switch(dir) {
			case NORTH_EAST:
				*pair.second += amount; // second is y
				*pair.first -= amount; // first is x
				break;
			case NORTH_WEST:
				*pair.second += amount;
				*pair.first += amount;
				break;
			case NORTH:
				*pair.second += amount;
				break;
			case SOUTH_EAST:
				*pair.second -= amount;
				*pair.first -= amount;
				break;
			case SOUTH_WEST:
				*pair.second -= amount;
				*pair.first += amount;
				break;
			case SOUTH:
				*pair.second -= amount;
				break;
			case WEST:
				*pair.first += amount;
				break;
			case EAST:
				*pair.first -= amount;
				break;
			default:
				break;
		}
  }
#ifdef PRINT_WORLD_COORDS
  std::cout << "plr: world_x: " << plr::self()->world_x << "\n";
  std::cout << "plr: world_y: " << plr::self()->world_y << "\n";
#endif
}

void MovementManager::wants_to_move(
    const World& world,
    Direction dir) {
	auto allowed = can_move(dir,plr::movement_amount());
	if(!allowed.first) {
		return;
	}

	Direction accepted_direction = (Direction)allowed.second;
	move_map(accepted_direction,plr::movement_amount());

	viewport::set_min_x(plr::get_rect()->x - win_width());
	viewport::set_max_x(plr::get_rect()->x + win_width());
	viewport::set_min_y(plr::get_rect()->y - win_height());
	viewport::set_max_y(plr::get_rect()->y + win_height());
	plr::calc();
}

namespace movement {
	static MovementManager* ptr;
	void force_move(Direction dir,int amount) {
		ptr->move_map(dir,amount);

		viewport::set_min_x(plr::get_rect()->x - win_width());
		viewport::set_max_x(plr::get_rect()->x + win_width());
		viewport::set_min_y(plr::get_rect()->y - win_height());
		viewport::set_max_y(plr::get_rect()->y + win_height());
		plr::calc();
	}
	void init(MovementManager* mgr) {
		ptr = mgr;
		auto start_tile = wall::start_tile();
    m_debug("start_tile: " << start_tile);
		int want_x = start_tile->self.rect.x;
		int want_y = start_tile->self.rect.y;
		int current_x = plr::get_rect()->x;
		int current_y = plr::get_rect()->y;
		if(current_x < want_x) {
			force_move(EAST,(want_x - current_x) / 2);
		} else {
			force_move(WEST,(current_x - want_x) / 2);
		}
		if(current_y < want_y) {
			force_move(SOUTH,(want_y - current_y) / 2);
		} else {
			force_move(NORTH,(current_y - want_y) / 2);
		}
	}
};
#undef m_debug
