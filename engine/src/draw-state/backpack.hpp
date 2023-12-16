#ifndef __DRAW_STATE__BACKPACK_HEADER__
#define __DRAW_STATE__BACKPACK_HEADER__
#include <iostream>
#include <SDL2/SDL.h>

namespace draw_state::backpack {
	void init();
  void tick();
	bool draw_backpack();
  void show_backpack();
  void hide_backpack();
  void draw_menu(SDL_Renderer* ren);
  bool editing_backpack();
  void start_menu();
};

#endif
