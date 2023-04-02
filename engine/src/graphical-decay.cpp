#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "graphical-decay.hpp"

namespace grdecay {
	int asset_id() {
		static int id = 0;
		return ++id;
	}

};
