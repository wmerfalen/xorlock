#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "movement.hpp"
#include "player.hpp"
#include "world.hpp"
#include "map.hpp"
#include "wall.hpp"
#include "air-support/f35.hpp"
#include "damage/explosions.hpp"

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
		if(SDL_TRUE == SDL_HasIntersection(&bubble,&wall->rect)) {
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
		if(SDL_TRUE == SDL_HasIntersection(ptr,&wall->rect)) {
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
	for(auto& n : world->npcs) {
		//if(npc::is_dead(n)) {
		if(dir == WEST) {
			n->rect.x += amount;
		} else if(dir == EAST) {
			n->rect.x -= amount;
		} else if(dir == NORTH) {
			n->rect.y += amount;
		} else if(dir == SOUTH) {
			n->rect.y -= amount;
		} else if(dir == NORTH_WEST) {
			n->rect.x += amount;
			n->rect.y += amount;
		} else if(dir == SOUTH_WEST) {
			n->rect.x += amount;
			n->rect.y -= amount;
		} else if(dir == NORTH_EAST) {
			n->rect.x -= amount;
			n->rect.y += amount;
		} else if(dir == SOUTH_EAST) {
			n->rect.x -= amount;
			n->rect.y -= amount;
		}
		//	continue;
		//}
		//switch(dir) {
		//	case EAST:
		//	case WEST:
		//		n->rect.x += adjustment;
		//		break;
		//	case NORTH:
		//	case SOUTH:
		//		n->rect.y += adjustment;
		//		break;
		//	case NORTH_EAST:
		//	case SOUTH_EAST:
		//	case NORTH_WEST:
		//	case SOUTH_WEST:
		//		n->rect.x += adjustment;
		//		n->rect.y += adjustment;
		//		break;
		//}
	}
	for(auto& wall : wall::walls) {
		switch(dir) {
			case NORTH_EAST:
				wall->rect.y += amount;
				wall->rect.x -= amount;
				break;
			case NORTH_WEST:
				wall->rect.y += amount;
				wall->rect.x += amount;
				break;
			case NORTH:
				wall->rect.y += amount;
				break;
			case SOUTH_EAST:
				wall->rect.y -= amount;
				wall->rect.x -= amount;
				break;
			case SOUTH_WEST:
				wall->rect.y -= amount;
				wall->rect.x += amount;
				break;
			case SOUTH:
				wall->rect.y -= amount;
				break;
			case WEST:
				wall->rect.x += amount;
				break;
			case EAST:
				wall->rect.x -= amount;
				break;
			default:
				break;
		}
	}
  air_support::f35::move_map(dir,amount);
  damage::explosions::move_map(dir,amount);
	//npc::spetsnaz_movement(dir,adjustment);
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
		int want_x = start_tile->rect.x;
		int want_y = start_tile->rect.y;
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
