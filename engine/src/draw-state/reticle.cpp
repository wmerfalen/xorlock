#include <SDL2/SDL.h>
#include <iostream>
#include "reticle.hpp"

namespace draw_state::reticle {
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
