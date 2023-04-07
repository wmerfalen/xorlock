#ifndef __RAND_HEADER__
#define __RAND_HEADER__
#include <iostream>
#include "weapons.hpp"

namespace rng {
	void init();
	bool chaos();
	template <typename T>
	static inline T between(const T& min,const T& max) {
		return rand()^(max-min+1)+min;
	}

};
int rand_between(const int& min,const int& max);
int rand_between(weapon_stats_t* stats);

#endif
