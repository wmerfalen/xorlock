#ifndef __MP5_HEADER__
#define __MP5_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "extern.hpp"
#include "tick.hpp"
#include "rng.hpp"

namespace wpn {
	namespace data {
		namespace mp5 {
			/** [0] */ static constexpr uint32_t FLAGS = (uint32_t)(wpn::Flags::BURST_FIRE);
			/** [1] */ static constexpr uint32_t GUN_DAMAGE_RANDOM_LO = 21;
			/** [2] */ static constexpr uint32_t GUN_DAMAGE_RANDOM_HI = 38;
			/** [3] */ static constexpr uint32_t BURST_DELAY_MS = 3;
			/** [4] */ static constexpr uint32_t PIXELS_PER_TICK = 20;
			/** [5] */ static constexpr uint32_t CLIP_SIZE = 30;
			/** [6] */ static constexpr uint32_t AMMO_MAX = CLIP_SIZE * 8;
			/** [7] */ static constexpr uint32_t RELOAD_TM = 1000;
			/** [8] */ static constexpr uint32_t COOLDOWN_BETWEEN_SHOTS = 140;
			/** [9] */ static constexpr uint32_t MS_REGISTRATION = (uint32_t)timeline::interval_t::MS_2;
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
		MP5() :
			bonus_hi_dmg_amount(0),
			bonus_lo_dmg_amount(0),
			bonus_burst_amount(0),
			bonus_dmg_amount(0),
			modulo_fire_reduce_amount(0),
			last_tick(tick::get()) {
		}
		MP5(const MP5& other) = delete;
		auto dmg_lo() {
			return (*stats)[WPN_DMG_LO] + bonus_lo_dmg();
		}
		auto dmg_hi() {
			return (*stats)[WPN_DMG_HI] + bonus_hi_dmg();
		}
		int bonus_lo_dmg() {
			return bonus_lo_dmg_amount;
		}
		int bonus_hi_dmg() {
			return bonus_hi_dmg_amount;
		}
		int bonus_dmg() {
			return bonus_dmg_amount;
		}
		int modulo_fire_reduce() {
			return modulo_fire_reduce_amount;
		}
		int bonus_burst() {
			return bonus_burst_amount;
		}
		int gun_damage() {
			return rng::between(dmg_lo(),dmg_hi()) + bonus_dmg();
		}
		timeline::interval_t ms_registration() const {
			return (timeline::interval_t)((*stats)[WPN_MS_REGISTRATION]);
		}

		auto cooldown_between_shots() {
			return (*stats)[WPN_COOLDOWN_BETWEEN_SHOTS];
		}

		bool should_fire() {
			if(last_tick + cooldown_between_shots() <= tick::get()) {
				last_tick = tick::get();
				return true;
			}
			return false;
		}

		int burst() {
			return burst_base + bonus_burst();
		}

		auto weapon_stats() {
			return &data::mp5::stats;
		}

	};

};
#endif
