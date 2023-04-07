#ifndef __WORLD_HEADER__
#define __WORLD_HEADER__
#include <forward_list>
#include "actor.hpp"
#include "hallway.hpp"

#include "viewport.hpp"

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

//namespace barrier {
//	bool top_intersects_with(Player& p);
//};

void draw_world();

#endif
