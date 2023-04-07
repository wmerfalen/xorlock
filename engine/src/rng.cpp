#include <iostream>
#include "rng.hpp"

namespace rng {
	void init() {
		// FIXME: use C++ random number generators
		srand(time(nullptr));
	}
	bool chaos() {
		return rand() > rand();
	}

};
int rand_between(const int& min,const int& max) {
	// FIXME: use C++ random number generators
	return rand()%(max-min + 1) + min;
}
int rand_between(weapon_stats_t* stats) {
	return rand_between((*stats)[WPN_DMG_LO],(*stats)[WPN_DMG_HI]);
}
