#ifndef __PLAYER_DRAW_STATE_HEADER__
#define __PLAYER_DRAW_STATE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include <map>
#include "extern.hpp"
#include "circle.hpp"
#include "npc-spetsnaz.hpp"
#include "mp5.hpp"
#include "cursor.hpp"
#include "bullet.hpp"
#include "draw.hpp"

namespace player_draw_state {
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
