#include <iostream>
#include "rng.hpp"

namespace rng {
	void init() {
		srand(time(nullptr));
	}
	bool chaos() {
		return rand() > rand();
	}

};
