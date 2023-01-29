#ifndef __DRAW_STATE__PLAYER_HEADER__
#define __DRAW_STATE__PLAYER_HEADER__
#include <iostream>

namespace draw_state::player {
	static bool m_draw_guy;
	void init() {
		m_draw_guy = 1;
	}
	bool draw_guy() {
		return m_draw_guy;
	}

	void hide_guy() {
		m_draw_guy = false;
	}
	void show_guy() {
		m_draw_guy = true;
	}

};

#endif
