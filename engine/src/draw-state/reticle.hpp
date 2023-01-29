#ifndef __DRAW_STATE__RETICLE_HEADER__
#define __DRAW_STATE__RETICLE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>

namespace draw_state::reticle {
	static bool m_draw_reticle;
	void init() {
		m_draw_reticle = 0;
	}
	bool draw_reticle() {
		return m_draw_reticle;
	}

	void hide_reticle() {
		m_draw_reticle = false;
	}
	void show_reticle() {
		m_draw_reticle = true;
	}

};

#endif
