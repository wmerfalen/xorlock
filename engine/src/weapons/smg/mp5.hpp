#ifndef __MP5_HEADER__
#define __MP5_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "../../weapons.hpp"
#include "../../tick.hpp"
#include "../../rng.hpp"
#include "../../timeline.hpp"

namespace weapons {
	namespace smg {
		namespace data {
			namespace mp5 {
				/** [0] */ static constexpr uint32_t FLAGS = (uint32_t)(wpn::Flags::BURST_FIRE);
				/** [1] */ static constexpr uint32_t GUN_DAMAGE_RANDOM_LO = 21;
				/** [2] */ static constexpr uint32_t GUN_DAMAGE_RANDOM_HI = 38;
				/** [3] */ static constexpr uint32_t BURST_DELAY_MS = 3;
				/** [4] */ static constexpr uint32_t PIXELS_PER_TICK = 30;
				/** [5] */ static constexpr uint32_t CLIP_SIZE = 30;
				/** [6] */ static constexpr uint32_t AMMO_MAX = CLIP_SIZE * 8;
				/** [7] */ static constexpr uint32_t RELOAD_TM = 1000;
				/** [8] */ static constexpr uint32_t COOLDOWN_BETWEEN_SHOTS = 60;
				/** [9] */ static constexpr uint32_t MS_REGISTRATION = (uint32_t)timeline::interval_t::MS_2;
        /** [10]*/ static constexpr uint32_t MAG_EJECT_TICKS = 950;
        /** [11]*/ static constexpr uint32_t PULL_REPLACEMENT_MAG_TICKS = 950;
        /** [12]*/ static constexpr uint32_t LOADING_MAG_TICKS = 950;
        /** [13]*/ static constexpr uint32_t SLIDE_PULL_TICKS = 950;
				static weapon_stats_t stats = {
					FLAGS,
					GUN_DAMAGE_RANDOM_LO,
					GUN_DAMAGE_RANDOM_HI,
					BURST_DELAY_MS,
					PIXELS_PER_TICK,
					CLIP_SIZE,
					AMMO_MAX,
					RELOAD_TM,
					COOLDOWN_BETWEEN_SHOTS,
					MS_REGISTRATION,
          MAG_EJECT_TICKS,
          PULL_REPLACEMENT_MAG_TICKS,
          LOADING_MAG_TICKS,
          SLIDE_PULL_TICKS,
				};
			};
		};
		struct MP5 {
			static constexpr weapon_stats_t * stats = &data::mp5::stats;
			static constexpr uint8_t burst_base = 3;
			int bonus_hi_dmg_amount;
			int bonus_lo_dmg_amount;
			int bonus_burst_amount;
			int bonus_dmg_amount;
			int modulo_fire_reduce_amount;
			uint64_t last_tick;
			uint64_t current_tick;
			uint16_t ammo;
			uint16_t total_ammo;
			MP5();
			MP5(const MP5& other) = delete;
			const uint16_t& consume_ammo();
			int dmg_lo();
			int dmg_hi();
			int bonus_lo_dmg();
			int bonus_hi_dmg();
			int bonus_dmg();
			int modulo_fire_reduce();
			int bonus_burst();
			int gun_damage();
			timeline::interval_t ms_registration() const;

			int cooldown_between_shots();

			bool should_fire();

			int burst();

			auto weapon_stats();

		};
	};

};
#endif
