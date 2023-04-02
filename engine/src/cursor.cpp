#include <iostream>
#include "cursor.hpp"


namespace cursor {
	SDL_Cursor* ptr = nullptr;
	SDL_Surface* surface = nullptr;
	int mouse_x;
	int mouse_y;
	void init() {
	}
	void use_reticle() {
		surface = SDL_LoadBMP("../assets/reticle-0.bmp");
		ptr = SDL_CreateColorCursor(surface,42,42);
		SDL_SetCursor(ptr);
	}
	void disable_cursor() {
		SDL_ShowCursor(SDL_DISABLE);
	}
	void update_mouse() {
		SDL_GetMouseState(&mouse_x,&mouse_y);
	}
	int& mx() {
		return mouse_x;
	}
	int& my() {
		return mouse_y;
	}
};
