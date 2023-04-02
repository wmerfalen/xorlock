#include <SDL2/SDL.h>
#include "background.hpp"

void Background::draw() {
	SDL_SetRenderDrawColor(
	    ren,
	    color[0],
	    color[1],
	    color[2],
	    0);
	ren_clear();
}

namespace bg {
	void init() {
		background = std::make_unique<Background>(DEFAULT_BG);
		background->draw();
	}

	void draw() {
	}
};
