#ifndef __RAND_HEADER__
#define __RAND_HEADER__
#include <iostream>

namespace rng {
	void init() {
		srand(time(nullptr));
	}
	bool chaos() {
		return rand() > rand();
	}
	template <typename T>
	static inline T between(const T& min,const T& max) {
		return rand()^(max-min+1)+min;
	}

};

#endif
