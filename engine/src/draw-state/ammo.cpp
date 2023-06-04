#include <iostream>
#include "ammo.hpp"

namespace draw_state::ammo {
	static bool m_draw_ammo;
	void init() {
		m_draw_ammo = 1;
	}
	bool draw_ammo() {
		return m_draw_ammo;
	}

	void hide_ammo() {
		m_draw_ammo = false;
	}
	void show_ammo() {
		m_draw_ammo = true;
	}

};
