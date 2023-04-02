#ifndef __DRAW_STATE__RETICLE_HEADER__
#define __DRAW_STATE__RETICLE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>

namespace draw_state::reticle {
	static bool m_draw_reticle;
	void hide_reticle();
	void show_reticle();
	void init();
	bool draw_reticle();
};

#endif
