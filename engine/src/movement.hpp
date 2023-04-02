#ifndef __MOVEMENT_HEADER__
#define __MOVEMENT_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "actor.hpp"
#include "player.hpp"
#include "world.hpp"
#include "extern.hpp"
namespace mg_static {
	static Player* p;
};

struct MovementManager {
	MovementManager() = default;
	~MovementManager() = default;
	int movement_amount;
	void move_map(Direction dir,int amount) {
		int adjustment = 0;
		switch(dir) {
			case NORTH:
				move_map_by(SOUTH,amount);
				if(mg_static::p->cy >= win_height() / 2) {
					movement_amount = 1;
					adjustment = -1;
				} else {
					movement_amount = -1;
					adjustment = 1;
				}
				break;
			case SOUTH:
				move_map_by(NORTH,amount);
				if(mg_static::p->cy <= win_height() / 2) {
					movement_amount = 1;
					adjustment = -1;
				} else {
					movement_amount = -1;
					adjustment = 1;
				}
				break;
			case WEST:
				move_map_by(EAST,amount);
				if(mg_static::p->cx >= win_width() / 2) {
					movement_amount = 1;
					adjustment = -1;
				} else {
					movement_amount = -1;
					adjustment = 1;
				}
				break;
			case EAST:
				move_map_by(WEST,amount);
				if(mg_static::p->cx <= win_width() / 2) {
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
	void wants_to_move(
	    const World& world,
	    Player& pl,
	    Direction dir) {
		mg_static::p = &pl;
		bool okay = true;
		if(!map::can_move(dir,pl.movement_amount)) {
			if((dir == WEST && !map::can_move(EAST,pl.movement_amount)) ||
			        (dir == EAST && !map::can_move(WEST,pl.movement_amount))  ||
			        (dir == NORTH && !map::can_move(SOUTH,pl.movement_amount))||
			        (dir == SOUTH && !map::can_move(NORTH,pl.movement_amount))) {
				okay = true;
			} else {
				okay = false;
			}
		}
		if(!okay) {
			return;
		}
		move_map(dir,pl.movement_amount);
		switch(dir) {
			case NORTH:
				pl.self.rect.y -= pl.movement_amount;
				break;
			case EAST:
				pl.self.rect.x += pl.movement_amount;
				break;
			case SOUTH:
				pl.self.rect.y += pl.movement_amount;
				break;
			case WEST:
				pl.self.rect.x -= pl.movement_amount;
				break;
		}
		viewport::min_x = pl.self.rect.x - win_width();
		viewport::max_x = pl.self.rect.x + win_width();
		viewport::min_y = pl.self.rect.y - win_height();
		viewport::max_y = pl.self.rect.y + win_height();
		pl.calc();
	}
};

#endif
