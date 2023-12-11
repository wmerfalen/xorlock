#ifndef __DRAW_STATE__BACKPACK_HEADER__
#define __DRAW_STATE__BACKPACK_HEADER__
#include <iostream>

namespace draw_state::backpack {
	void init();
  void tick();
	bool draw_backpack();
  void show_backpack();
  void hide_backpack();
};

#endif
