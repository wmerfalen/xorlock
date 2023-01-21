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
				movement_amount = amount;
				break;
			case SOUTH:
				move_map_by(NORTH,amount);
				movement_amount = amount;
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
		}
		for(auto& n : world->npcs) {
			if(npc::is_dead(n)) {
				if(dir == WEST) {
					n->rect.x += amount;
				} else if(dir == EAST) {
					n->rect.x -= amount;
				}
				continue;
			}
			n->rect.x  += adjustment;
		}
		npc::spetsnaz_movement(adjustment);
	}
	int get_movement_amount() {
		return movement_amount;
	}
	void wants_to_move(
	    const World& world,
	    Player& pl,
	    Direction dir) {
		mg_static::p = &pl;
		move_map(dir,pl.movement_amount);
		switch(dir) {
			case NORTH:
				pl.self.rect.y -= get_movement_amount();
				if(barrier::top_intersects_with(pl)) {
					pl.self.rect.y += get_movement_amount();
					break;
				}
				break;
			case EAST:
				pl.self.rect.x += get_movement_amount();
				if(barrier::top_intersects_with(pl)) {
					pl.self.rect.x -= get_movement_amount();
					break;
				}
				break;
			case SOUTH:
				pl.self.rect.y += get_movement_amount();
				if(barrier::top_intersects_with(pl)) {
					pl.self.rect.y += get_movement_amount();
					break;
				}
				break;
			case WEST:
				pl.self.rect.x -= get_movement_amount();
				if(barrier::top_intersects_with(pl)) {
					pl.self.rect.x += get_movement_amount();
					break;
				}
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
