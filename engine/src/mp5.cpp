#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "extern.hpp"
#include "mp5.hpp"
#include "rng.hpp"

namespace wpn {
	int MP5::dmg_lo() {
		return (*stats)[WPN_DMG_LO] + bonus_lo_dmg();
	}
	int MP5::dmg_hi() {
		return (*stats)[WPN_DMG_HI] + bonus_hi_dmg();
	}
	int MP5::bonus_lo_dmg() {
		return bonus_lo_dmg_amount;
	}
	int MP5::bonus_hi_dmg() {
		return bonus_hi_dmg_amount;
	}
	int MP5::bonus_dmg() {
		return bonus_dmg_amount;
	}
	int MP5::modulo_fire_reduce() {
		return modulo_fire_reduce_amount;
	}
	int MP5::bonus_burst() {
		return bonus_burst_amount;
	}
	int MP5::gun_damage() {
		return rng::between(dmg_lo(),dmg_hi()) + bonus_dmg();
	}
	timeline::interval_t MP5::ms_registration() const {
		return (timeline::interval_t)((*stats)[WPN_MS_REGISTRATION]);
	}

	int MP5::cooldown_between_shots() {
		return (*stats)[WPN_COOLDOWN_BETWEEN_SHOTS];
	}

	bool MP5::should_fire() {
		if(last_tick + cooldown_between_shots() <= tick::get()) {
			last_tick = tick::get();
			return true;
		}
		return false;
	}

	int MP5::burst() {
		return burst_base + bonus_burst();
	}

	auto MP5::weapon_stats() {
		return &data::mp5::stats;
	}

};
