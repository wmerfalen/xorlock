#ifndef __WORLD_HEADER__
#define __WORLD_HEADER__
#include <forward_list>
#include "actor.hpp"
#include "hallway.hpp"
#include "extern.hpp"
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

void move_map_by(int dir, int amount) {
	Direction d = (Direction)dir;
	int adjustment = 0;
	switch(d) {
		case SOUTH:
		case NORTH:
			return;
		case WEST:
			world->x -= amount;
			adjustment = - 1 * amount;
			break;
		case EAST:
			world->x += amount;
			adjustment = abs(amount);
			break;
	}
	for(auto& h : world->halls) {
		h.self.rect.x += adjustment;
	}
	npc::spetsnaz_movement(adjustment);
}

void init_world() {
	world->halls.emplace_front(HORIZONTAL,0,0,"../assets/hallway-ltr-wall-%d.bmp");
}

namespace barrier {
	int hall_width = 70;
	int hall_height = 80;
	SDL_Rect top = {0,0,win_width(),hall_height};
	template <typename TPlayer>
	bool top_intersects_with(TPlayer& p) {
		return SDL_HasIntersection(&top,&p.self.rect);
	}
};

void draw_world() {

	/*
	 * Phase 1 goal: Draw eternal hallways going LEFT_TO_RIGHT
	 * - [ ] complete? >:(
	 */
	for(auto& h : world->halls) {
		for(auto& b : h.self.bmp) {
			SDL_RenderCopy(ren, b.texture, nullptr, &h.self.rect);
			int i=1;
			while(((h.self.rect.w * i) + h.self.rect.x) < (win_width() + 120)) {
				auto d = h.self.rect;
				d.x = h.self.rect.x + (h.self.rect.w * i);
				SDL_RenderCopy(ren, b.texture, nullptr, &d);
				++i;
			}
		}
	}
}

#endif
