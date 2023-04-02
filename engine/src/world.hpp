#ifndef __WORLD_HEADER__
#define __WORLD_HEADER__
#include <forward_list>
#include "actor.hpp"
#include "hallway.hpp"
#include "extern.hpp"
#include "viewport.hpp"
#include "hallway.hpp"

struct World {
	uint64_t width;
	uint64_t height;
	int x;
	int y;

	std::forward_list<Actor*> npcs;
	std::forward_list<Hallway> halls;
};

extern std::unique_ptr<World> world;
enum Direction : uint8_t {
	NORTH,
	EAST,
	WEST,
	SOUTH,
};

void move_map_by(int dir, int amount);

void init_world();

namespace barrier {
	static int hall_width = 70;
	static int hall_height = 80;
	static SDL_Rect top = {0,0,win_width(),hall_height};
	template <typename TPlayer>
	static inline bool top_intersects_with(TPlayer& p) {
		return SDL_HasIntersection(&top,&p.self.rect);
	}
};

void draw_world();

#endif
