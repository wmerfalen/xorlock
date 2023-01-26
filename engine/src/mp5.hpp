#ifndef __MP5_HEADER__
#define __MP5_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "extern.hpp"
#include "tick.hpp"
#include "rng.hpp"

namespace wpn {
	struct MP5 {
		/**
		 * Burst shot of 3 bullets
		 */
		static constexpr uint32_t FLAGS = (uint32_t)(wpn::Flags::BURST_FIRE);
		static constexpr uint32_t GUN_DAMAGE_RANDOM_LO = 2;
		static constexpr uint32_t GUN_DAMAGE_RANDOM_HI = 6;
		static constexpr uint32_t BURST_DELAY_MS = 3;
		static constexpr uint32_t PIXELS_PER_TICK = 20;
		static constexpr uint32_t CLIP_SIZE = 30;
		static constexpr uint32_t AMMO_MAX = CLIP_SIZE * 8;
		static constexpr uint32_t COOLDOWN_BETWEEN_SHOTS = 180;
		weapon_stats_t stats = {
			FLAGS,
			GUN_DAMAGE_RANDOM_LO,
			GUN_DAMAGE_RANDOM_HI,
			BURST_DELAY_MS,
			PIXELS_PER_TICK,
			CLIP_SIZE,
			AMMO_MAX,
			COOLDOWN_BETWEEN_SHOTS,
		};

		MP5() : last_tick(tick::get()) {}
		int gun_damage() {
			return rng::between(GUN_DAMAGE_RANDOM_LO,GUN_DAMAGE_RANDOM_HI);
		}

		MP5(const MP5& other) = delete;
		uint64_t last_tick;
		uint64_t current_tick;

		bool should_fire() {
			current_tick = tick::get();
			if(last_tick + COOLDOWN_BETWEEN_SHOTS <= current_tick) {
				last_tick = current_tick;
				return true;
			}
			return false;
		}

		uint8_t burst() const {
			return 3;
		}

		auto weapon_stats() {
			return &stats;
		}

	};

};
#endif
