#ifndef __MOVEMENT_HEADER__
#define __MOVEMENT_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "actor.hpp"
#include "player.hpp"
#include "world.hpp"
#include "direction.hpp"

struct MovementManager {
	MovementManager() = default;
	~MovementManager() = default;
	int movement_amount;
	std::pair<bool,uint8_t> can_move(int direction,int amount);
	void move_map(Direction dir,int amount);
	void wants_to_move(
	    const World& world,
	    Direction dir);
};

bool can_move_direction(int direction,SDL_Rect* p,int adjustment);
#endif
