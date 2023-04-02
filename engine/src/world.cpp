#include <forward_list>
#include "actor.hpp"
#include "world.hpp"
#include "extern.hpp"

void move_map_by(int dir, int amount) {
	Direction d = (Direction)dir;
	int adjustment = 0;
	switch(d) {
		case SOUTH:
			world->y += amount;
			adjustment = abs(amount);
			break;
		case NORTH:
			world->y -= amount;
			adjustment = - 1 * amount;
			break;
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
		if(dir == EAST || dir == WEST) {
			h.self.rect.x += adjustment;
		} else {
			h.self.rect.y  += adjustment;
		}
	}
	npc::spetsnaz_movement(dir,adjustment);
}

void init_world() {
	world->halls.emplace_front(HORIZONTAL,0,0,"../assets/hallway-ltr-wall-%d.bmp");
}

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
