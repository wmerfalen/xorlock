#ifndef __MAP_HEADER__
#define __MAP_HEADER__
#include "extern.hpp"
#include "actor.hpp"
#include "world.hpp"
#include "coordinates.hpp"
#include "viewport.hpp"
#include "circle.hpp"
#include "rng.hpp"
#include "draw.hpp"
#include <vector>
#include <functional>
#include <memory>

namespace map {
	struct Wall {
		SDL_Rect rect;
		bool initialized;
		Wall(
		    const int& _x,
		    const int& _y,
		    const int& _width,
		    const int& _height);
		Wall() : initialized(false) {}
		Wall(const Wall& o) = delete;
		~Wall() = default;
		void render();
	};// end Wall
	static std::vector<std::unique_ptr<Wall>> walls;
	SDL_Rect collision;
	bool can_move(int direction,int amount);
	void move_map(int direction,int amount);
	void tick();
	void init();
};

#endif
