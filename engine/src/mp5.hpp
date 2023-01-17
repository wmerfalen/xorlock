#ifndef __MP5_HEADER__
#define __MP5_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
//#include <array>
//#include <string_view>
//#include <memory>
//#include "actor.hpp"
//#include "world.hpp"
//#include "triangle.hpp"
//#include "coordinates.hpp"
//#include "bullet-pool.hpp"
//#include <map>
#include "extern.hpp"
#include "tick.hpp"
//#include "circle.hpp"
//#include "npc-spetsnaz.hpp"

namespace wpn {
	struct MP5 {
		/**
		 * Burst shot of 3 bullets
		 */
		static constexpr uint16_t GUN_DAMAGE_RANDOM_LO = 2;
		static constexpr uint16_t GUN_DAMAGE_RANDOM_HI = 6;
		static constexpr uint16_t ROUNDS_PER_SECOND = 13;
		static constexpr uint16_t PIXELS_PER_TICK = 20;
		static constexpr uint8_t CLIP_SIZE = 30;
		static constexpr uint16_t AMMO_MAX = CLIP_SIZE * 8;
		static constexpr uint32_t COOLDOWN_BETWEEN_SHOTS = 40;

		MP5() : last_tick(tick::get()) {}
		int gun_damage() {
			return rand_between(GUN_DAMAGE_RANDOM_LO,GUN_DAMAGE_RANDOM_HI);
		}

		MP5(const MP5& other) = delete;
		uint64_t last_tick;

		bool should_fire() {
			if(last_tick + COOLDOWN_BETWEEN_SHOTS <= tick::get()) {
				last_tick = tick::get();
				return true;
			}
			return false;
		}

		uint8_t burst() const {
			return 3;
		}

	};

};
#endif
