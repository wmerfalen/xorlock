#ifndef __DRAW_STATE__PLAYER_HEADER__
#define __DRAW_STATE__PLAYER_HEADER__
#include <iostream>

namespace draw_state::player {
	void init();
	bool draw_guy();
  void tick();

	void hide_guy();
	void show_guy();
  void show_hp(bool on_off);
};

#endif
