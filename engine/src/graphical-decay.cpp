#ifndef __GRAPHICAL_DECAY_HEADER__
#define __GRAPHICAL_DECAY_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "clock.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "extern.hpp"

namespace grdecay {
	struct asset {
		int id;
		bool done;
		bool run_me;
		uint16_t when;
		int ctr;
		timeline::callback_t func;
	};

	int asset_id() {
		static int id = 0;
		return ++id;
	}

};

#endif
