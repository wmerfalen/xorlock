#ifndef __WORLD_HEADER__
#define __WORLD_HEADER__
#include <forward_list>
#include "actor.hpp"
#include "hallway.hpp"

#include "viewport.hpp"

static constexpr int CELL_WIDTH = 95;
static constexpr int CELL_HEIGHT = 95;
struct World {
	uint64_t width;
	uint64_t height;
	int x;
	int y;

	std::forward_list<Actor*> npcs;
};
void cleanup_dead_npcs(const std::vector<Actor*>& corpses);

extern std::unique_ptr<World> world;

void move_map_by(int dir, int amount);

void init_world(const std::string& level);


int import_tiled_world(const std::string&  _world_csv);

void world_tick();

void world_program_exit();

#endif
