#include "../extern.hpp"
#include "init.hpp"
#include "player.hpp"
#include "reticle.hpp"
#include "backpack.hpp"

namespace draw_state {
	void init() {
		player::init();
		reticle::init();
    backpack::init();
	}

};
