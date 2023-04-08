#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "movement.hpp"
#include "player.hpp"
#include "world.hpp"
#include "map.hpp"
#include "wall.hpp"

void move_map_by(int dir, int amount) {
	Direction d = (Direction)dir;
	int adjustment = 0;
	switch(d) {
		case SOUTH:
			world->y += amount;
			adjustment = abs(amount);
			break;
		case NORTH:
			world->y -= amount;
			adjustment = - 1 * amount;
			break;
		case WEST:
			world->x -= amount;
			adjustment = - 1 * amount;
			break;
		case EAST:
			world->x += amount;
			adjustment = abs(amount);
			break;
		default:
			break;
	}
	npc::spetsnaz_movement(dir,adjustment);
}
bool MovementManager::can_move(int direction,int amount) {
	SDL_Rect result, *p;
	static int adjustment = plr::movement_amount();
	p = plr::get_effective_move_rect();
	plr::draw_collision_outline(p);
	for(const auto& wall : wall::walls) {
		if(wall->walkable) {
			continue;
		}
		bool can_before_adjustment = !SDL_IntersectRect(
		                                 &wall->rect,
		                                 p,
		                                 &result);
		if(!can_before_adjustment) {
			plr::restore_collision_outline(&result);
		}
		switch(direction) {
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
			move_map_by(SOUTH,amount);
			if(plr::cy() >= win_height() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			break;
		case SOUTH:
			move_map_by(NORTH,amount);
			if(plr::cy() <= win_height() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			break;
		case WEST:
			move_map_by(EAST,amount);
			if(plr::cx() >= win_width() / 2) {
				movement_amount = 1;
				adjustment = -1;
			} else {
				movement_amount = -1;
				adjustment = 1;
			}
			break;
		case EAST:
			move_map_by(WEST,amount);
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
			}
			continue;
		}
		if(dir == EAST || dir == WEST) {
			n->rect.x  += adjustment;
		} else {
			n->rect.y += adjustment;
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
	npc::spetsnaz_movement(dir,adjustment);
}
void MovementManager::wants_to_move(
    const World& world,
    Direction dir) {
	bool okay = true;
	if(!can_move(dir,plr::movement_amount())) {
		if((dir == WEST && !can_move(EAST,plr::movement_amount())) ||
		        (dir == EAST && !can_move(WEST,plr::movement_amount()))  ||
		        (dir == NORTH && !can_move(SOUTH,plr::movement_amount()))||
		        (dir == SOUTH && !can_move(NORTH,plr::movement_amount()))) {
			okay = true;
		} else {
			okay = false;
		}
	}
	if(!okay) {
		return;
	}
	move_map(dir,plr::movement_amount());
	switch(dir) {
		case NORTH:
			plr::get_rect()->y -= plr::movement_amount();
			break;
		case EAST:
			plr::get_rect()->x += plr::movement_amount();
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
	viewport::set_min_x(plr::get_rect()->x - win_width());
	viewport::set_max_x(plr::get_rect()->x + win_width());
	viewport::set_min_y(plr::get_rect()->y - win_height());
	viewport::set_max_y(plr::get_rect()->y + win_height());
	viewport::report();
	plr::calc();
}
