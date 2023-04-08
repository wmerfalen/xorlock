#ifndef __WORLD_HEADER__
#define __WORLD_HEADER__
#include <forward_list>
#include "actor.hpp"
#include "hallway.hpp"

#include "viewport.hpp"

static constexpr int CELL_WIDTH = 80;
static constexpr int CELL_HEIGHT = 80;
struct World {
	uint64_t width;
	uint64_t height;
	int x;
	int y;

	std::forward_list<Actor*> npcs;
	//std::forward_list<Hallway> halls;
};

extern std::unique_ptr<World> world;

void move_map_by(int dir, int amount);

void init_world();

void draw_world();

int import_tiled_world(const std::string&  _world_csv);

#endif
