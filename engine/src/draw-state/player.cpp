#include <iostream>
#include "player.hpp"

namespace draw_state::player {
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
