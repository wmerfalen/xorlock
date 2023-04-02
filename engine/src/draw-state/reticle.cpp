#ifndef __DRAW_STATE__RETICLE_HEADER__
#define __DRAW_STATE__RETICLE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>

namespace draw_state::reticle {
	static bool m_draw_reticle;
	void hide_reticle() {
		m_draw_reticle = false;
	}
	void show_reticle() {
		m_draw_reticle = true;
	}
	void init() {
#ifdef SHOW_RETICLE
		show_reticle();
#else
		hide_reticle();
#endif
	}
	bool draw_reticle() {
		return m_draw_reticle;
	}


};

#endif
