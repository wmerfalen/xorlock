#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "movement.hpp"
#include "player.hpp"
#include "world.hpp"
#include "map.hpp"
#include "wall.hpp"

bool MovementManager::can_move(int direction,int amount) {
	SDL_Rect result, *p;
	static int adjustment = plr::movement_amount() + (plr::movement_amount() * 1.03);
	for(const auto& wall : wall::walls) {
		if(wall->walkable) {
			continue;
		}
		p = plr::get_effective_move_rect();
		plr::draw_collision_outline(p);
		bool can_before_adjustment = !SDL_IntersectRect(
		                                 &wall->rect,
		                                 p,
		                                 &result);
		if(!can_before_adjustment) {
			plr::restore_collision_outline(&result);
		}
		switch(direction) {
			case SOUTH_WEST:
				p->x -= adjustment;
				p->y += adjustment;
				break;
			case NORTH_WEST:
				p->x -= adjustment;
				p->y -= adjustment;
				break;
			case SOUTH_EAST:
				p->x += adjustment;
				p->y += adjustment;
				break;
			case NORTH_EAST:
				p->x += adjustment;
				p->y -= adjustment;
				break;
			case NORTH:
				p->y -= adjustment;
				break;
			case SOUTH:
				p->y += adjustment;
				break;
			case WEST:
				p->x -= adjustment;
				break;
			case EAST:
				p->x += adjustment;
				break;
			default:
				break;
		}
		bool can_after_adjustment = !SDL_IntersectRect(
		                                &wall->rect,
		                                p,
		                                &result);
		if(!can_before_adjustment && !can_after_adjustment) {
			return false;
		}
	}
	return true;
}
void MovementManager::move_map(Direction dir,int amount) {
	int adjustment = 0;
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
			break;
		case NORTH_EAST:
			std::cout << "NORTH_EAST\n";
			world->x += amount;
			world->y -= amount;
			adjustment = abs(amount);
			if(plr::cx() <= win_width() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
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
			break;
		case SOUTH_EAST:
			std::cout << "SOUTH_EAST\n";
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
			break;
		default:
			break;
	}
	for(auto& n : world->npcs) {
		if(npc::is_dead(n)) {
			if(dir == WEST) {
				n->rect.x += amount;
			} else if(dir == EAST) {
				n->rect.x -= amount;
			} else if(dir == NORTH) {
				n->rect.y += amount;
			} else if(dir == SOUTH) {
				n->rect.y -= amount;
			} else if(dir == NORTH_EAST) {
				n->rect.x += amount;
				n->rect.y -= amount;
			} else if(dir == SOUTH_EAST) {
				n->rect.x += amount;
				n->rect.y += amount;
			}
			continue;
		}
		switch(dir) {
			case EAST:
			case WEST:
				n->rect.x += adjustment;
				break;
			case NORTH:
			case SOUTH:
				n->rect.y += adjustment;
				break;
			case NORTH_EAST:
			case SOUTH_EAST:
			case NORTH_WEST:
			case SOUTH_WEST:
				n->rect.x += adjustment;
				n->rect.y += adjustment;
				break;
		}
	}
	for(auto& wall : wall::walls) {
		switch(dir) {
			case NORTH:
				wall->rect.y += amount;
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
	//npc::spetsnaz_movement(dir,adjustment);
}
void MovementManager::wants_to_move(
    const World& world,
    Direction dir) {
	bool okay = true;
	if(!can_move(dir,plr::movement_amount())) {
		return;
	}
	move_map(dir,plr::movement_amount());
	switch(dir) {
		case NORTH_EAST:
			plr::get_rect()->y -= plr::movement_amount();
			plr::get_rect()->x += plr::movement_amount();
			break;
		case NORTH_WEST:
			plr::get_rect()->y -= plr::movement_amount();
			plr::get_rect()->x -= plr::movement_amount();
			break;
		case NORTH:
			plr::get_rect()->y -= plr::movement_amount();
			break;
		case EAST:
			plr::get_rect()->x += plr::movement_amount();
			break;
		case SOUTH_EAST:
			plr::get_rect()->y += plr::movement_amount();
			plr::get_rect()->x += plr::movement_amount();
			break;
		case SOUTH_WEST:
			plr::get_rect()->y += plr::movement_amount();
			plr::get_rect()->x -= plr::movement_amount();
			break;
		case SOUTH:
			plr::get_rect()->y += plr::movement_amount();
			break;
		case WEST:
			plr::get_rect()->x -= plr::movement_amount();
			break;
	}
#ifdef DISPLAY_PLAYER_X_Y
	std::cout << plr::get_rect()->x << "x" << plr::get_rect()->y << "\n";
#endif
	viewport::set_min_x(plr::get_rect()->x - win_width() / 2);
	viewport::set_max_x(plr::get_rect()->x + win_width() / 2);
	viewport::set_min_y(plr::get_rect()->y - win_height() / 2);
	viewport::set_max_y(plr::get_rect()->y + win_height() / 2);
	//viewport::report();
	plr::calc();
}
