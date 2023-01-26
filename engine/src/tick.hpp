#ifndef __TICK_HEADER__
#define __TICK_HEADER__
#include <iostream>
#include "extern.hpp"

namespace tick {
	struct Tick {
		uint64_t get() const {
			return current;
		}
		uint64_t inc() {
			++current;
			return current;
		}
		Tick() : current(0) {}
		Tick(const Tick& o) = delete;
		~Tick() = default;
		uint64_t current;
	};

	static Tick impl;
	auto get() {
		return impl.current;
	}
	auto inc() {
		return impl.inc();
	}
	void init() {
		impl.inc();
	}
};

#endif
