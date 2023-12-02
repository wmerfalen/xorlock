#ifndef __GAMEPLAY_HEADER__
#define __GAMEPLAY_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"

#include "npc-spetsnaz.hpp"
#include "gameplay/npc-spawning.hpp"

namespace gameplay {
	void init();
	void tick();
	void numeric_pressed(uint8_t v);
	bool needs_numeric();
  void toggle_menu();
  void show_pause_menu();
  void hide_pause_menu();
  bool game_is_paused();
  void handle_key_press();
  void draw_pause_menu(SDL_Renderer* ren);
}; // end namespace gameplay


#endif
