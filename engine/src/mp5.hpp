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
			static constexpr uint32_t FLAGS = (uint32_t)(wpn::Flags::BURST_FIRE);
			static constexpr uint32_t GUN_DAMAGE_RANDOM_LO = 2;
			static constexpr uint32_t GUN_DAMAGE_RANDOM_HI = 6;
			static constexpr uint32_t BURST_DELAY_MS = 3;
			static constexpr uint32_t PIXELS_PER_TICK = 20;
			static constexpr uint32_t CLIP_SIZE = 30;
			static constexpr uint32_t AMMO_MAX = CLIP_SIZE * 8;
			static constexpr uint32_t COOLDOWN_BETWEEN_SHOTS = 180;
			static weapon_stats_t stats = {
				FLAGS,
				GUN_DAMAGE_RANDOM_LO,
				GUN_DAMAGE_RANDOM_HI,
				BURST_DELAY_MS,
				PIXELS_PER_TICK,
				CLIP_SIZE,
				AMMO_MAX,
				COOLDOWN_BETWEEN_SHOTS,
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
		int cooldown_tick_reduce_amount;
		uint64_t last_tick;
		uint64_t current_tick;
		MP5() :
			bonus_hi_dmg_amount(0),
			bonus_lo_dmg_amount(0),
			bonus_burst_amount(0),
			bonus_dmg_amount(0),
			cooldown_tick_reduce_amount(0),
			last_tick(tick::get()) {
		}
		MP5(const MP5& other) = delete;
		auto dmg_lo() {
			return (*stats)[WPN_DMG_LO] + bonus_lo_dmg();
		}
		auto dmg_hi() {
			return (*stats)[WPN_DMG_HI] + bonus_hi_dmg();
		}
		auto cooldown_between_shots() {
			return (*stats)[WPN_COOLDOWN] - cooldown_tick_reduce();
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
		int cooldown_tick_reduce() {
			return cooldown_tick_reduce_amount;
		}
		int bonus_burst() {
			return bonus_burst_amount;
		}
		int gun_damage() {
			return rng::between(dmg_lo(),dmg_hi()) + bonus_dmg();
		}

		bool should_fire() {
			current_tick = tick::get();
			if(last_tick + cooldown_between_shots() <= current_tick) {
				last_tick = current_tick;
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
