#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "movement.hpp"
#include "player.hpp"
#include "world.hpp"
#include "map.hpp"
#include "extern.hpp"
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
	npc::spetsnaz_movement(dir,adjustment);
	map::move_map(dir,abs(amount));
}
void MovementManager::wants_to_move(
    const World& world,
    Direction dir) {
	bool okay = true;
	if(!map::can_move(dir,plr::movement_amount())) {
		if((dir == WEST && !map::can_move(EAST,plr::movement_amount())) ||
		        (dir == EAST && !map::can_move(WEST,plr::movement_amount()))  ||
		        (dir == NORTH && !map::can_move(SOUTH,plr::movement_amount()))||
		        (dir == SOUTH && !map::can_move(NORTH,plr::movement_amount()))) {
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
	plr::calc();
}
