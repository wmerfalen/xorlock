#include "debug.hpp"
#include <SDL2/SDL.h>


namespace dbg {

	std::string dump(SDL_Rect* r) {
		std::string s = "x: ";
		s += std::to_string(r->x) + "\n";
		s += "y: " + std::to_string(r->y) + "\n";
		s += "w: " + std::to_string(r->w) + "\n";
		s += "h: " + std::to_string(r->h) + "\n";
		return s;
	}

};
