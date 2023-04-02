#ifndef __MOVEMENT_HEADER__
#define __MOVEMENT_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "actor.hpp"
#include "player.hpp"
#include "world.hpp"
#include "extern.hpp"

struct MovementManager {
	MovementManager() = default;
	~MovementManager() = default;
	int movement_amount;
	void move_map(Direction dir,int amount);
	void wants_to_move(
	    const World& world,
	    Direction dir);
};

#endif
