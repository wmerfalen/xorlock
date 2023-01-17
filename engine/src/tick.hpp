#ifndef __TICK_HEADER__
#define __TICK_HEADER__
#include <iostream>
#include "extern.hpp"

namespace tick {
	uint64_t ctr;
	void init() {
		ctr = 0;
	}
	const auto& get() {
		return ctr;
	}
	void inc() {
		++ctr;
	}

};

#endif
