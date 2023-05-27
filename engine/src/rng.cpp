#include <iostream>
#include <random>
#include <chrono>
#include "rng.hpp"

#ifdef  RAND_DEBUG
#define rand_debug(a) "[DEBUG][" << __FILE__ << ":" << __LINE__ << "]:" << a << "\n";
#else
#define rand_debug(a) ;;
#endif
namespace rng {
	bool chance(uint8_t percent) {
		uint8_t f = xoroshiro::next() % 100;
		return percent <= percent;
	}
	uint64_t next() {
		return rng::xoroshiro::next();
	}
	namespace xoroshiro {
		/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

		To the extent possible under law, the author has dedicated all copyright
		and related and neighboring rights to this software to the public domain
		worldwide. This software is distributed without any warranty.

		See <http://creativecommons.org/publicdomain/zero/1.0/>. */

#include <stdint.h>

		/* This is xoshiro256** 1.0, one of our all-purpose, rock-solid
			 generators. It has excellent (sub-ns) speed, a state (256 bits) that is
			 large enough for any parallel application, and it passes all tests we
			 are aware of.

			 For generating just floating-point numbers, xoshiro256+ is even faster.

			 The state must be seeded so that it is not everywhere zero. If you have
			 a 64-bit seed, we suggest to seed a splitmix64 generator and use its
			 output to fill s. */

		static inline uint64_t rotl(const uint64_t x, uint32_t k) {
			return (x << k) | (x >> (64 - k));
		}


		static uint64_t s[4];

		uint64_t next(void) {
			const uint64_t result = rotl(s[1] * 5, 7) * 9;

			const uint64_t t = s[1] << 17;

			s[2] ^= s[0];
			s[3] ^= s[1];
			s[1] ^= s[2];
			s[0] ^= s[3];

			s[2] ^= t;

			s[3] = rotl(s[3], 45);

			return result;
		}


		/* This is the jump function for the generator. It is equivalent
			 to 2^128 calls to next(); it can be used to generate 2^128
			 non-overlapping subsequences for parallel computations. */

		void jump(void) {
			static const uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

			uint64_t s0 = 0;
			uint64_t s1 = 0;
			uint64_t s2 = 0;
			uint64_t s3 = 0;
			for(uint32_t i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
				for(uint32_t b = 0; b < 64; b++) {
					if(JUMP[i] & UINT64_C(1) << b) {
						s0 ^= s[0];
						s1 ^= s[1];
						s2 ^= s[2];
						s3 ^= s[3];
					}
					next();
				}

			s[0] = s0;
			s[1] = s1;
			s[2] = s2;
			s[3] = s3;
		}



		/* This is the long-jump function for the generator. It is equivalent to
			 2^192 calls to next(); it can be used to generate 2^64 starting points,
			 from each of which jump() will generate 2^64 non-overlapping
			 subsequences for parallel distributed computations. */

		void long_jump(void) {
			static const uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

			uint64_t s0 = 0;
			uint64_t s1 = 0;
			uint64_t s2 = 0;
			uint64_t s3 = 0;
			for(uint32_t i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
				for(uint32_t b = 0; b < 64; b++) {
					if(LONG_JUMP[i] & UINT64_C(1) << b) {
						s0 ^= s[0];
						s1 ^= s[1];
						s2 ^= s[2];
						s3 ^= s[3];
					}
					next();
				}

			s[0] = s0;
			s[1] = s1;
			s[2] = s2;
			s[3] = s3;
		}
	};//end namespace mods::rand::xoroshiro
	void init() {
		// FIXME: use C++ random number generators
		srand(time(nullptr));
		using namespace std::chrono;
		for(uint8_t i = 0; i < 4; i++) {
			xoroshiro::s[i] = duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count();
		}
	}
	bool chaos() {
		return rand() > rand();
	}

};
int rand_between(const int& min,const int& max) {
	std::cerr << "rand_between: " << min << " max: " << max <<"\n";
	return rng::xoroshiro::next()%(max-min + 1) + min;
}
int rand_between(weapon_stats_t* stats) {
	return rand_between((*stats)[WPN_DMG_LO],(*stats)[WPN_DMG_HI]);
}

uint64_t rand_xoroshiro() {
	return rng::xoroshiro::next();
}
