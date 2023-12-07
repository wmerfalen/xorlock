#ifndef __DRAW_STATE__PLAYER_HEADER__
#define __DRAW_STATE__PLAYER_HEADER__
#include <iostream>

namespace draw_state::player {
	static bool m_draw_guy;
	void init();
	bool draw_guy();

	void hide_guy();
	void show_guy();
};

#endif
