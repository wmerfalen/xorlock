#ifndef __RAND_HEADER__
#define __RAND_HEADER__
#include <iostream>
#include <random>
#include <algorithm>
#include "weapons.hpp"

namespace rng {
	namespace xoroshiro {
		uint64_t next(void);
	};
	void init();
	bool chaos();
	template <typename T>
	static inline T between(const T& min,const T& max) {
		return xoroshiro::next() % ((max-min)+1) + min;
	}
	uint64_t next();
	template <typename T>
	T max(const T& _m) {
		return xoroshiro::next() % _m;
	}
	bool chance(uint8_t percent);

};
int rand_between(const int& min,const int& max);
int rand_between(weapon_stats_t* stats);
template <typename T>
static inline T shuffle_container(const T& v) {
	std::random_device rd;
	std::mt19937 g(rd());
	auto f = v;
	std::shuffle(f.begin(), f.end(), g);
	return f;
}
template <typename T>
static inline const auto& random_item(const T& container) {
	return container[0];
}
uint64_t rand_xoroshiro();
#endif
