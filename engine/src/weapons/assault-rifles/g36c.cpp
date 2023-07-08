#include <SDL2/SDL.h>
#include <iostream>
#include <array>

#include "g36c.hpp"
#include "../../rng.hpp"

namespace wpn {
	int G36C::dmg_lo() {
		return (*stats)[WPN_DMG_LO] + bonus_lo_dmg();
	}
	int G36C::dmg_hi() {
		return (*stats)[WPN_DMG_HI] + bonus_hi_dmg();
	}
	int G36C::bonus_lo_dmg() {
		return bonus_lo_dmg_amount;
	}
	int G36C::bonus_hi_dmg() {
		return bonus_hi_dmg_amount;
	}
	int G36C::bonus_dmg() {
		return bonus_dmg_amount;
	}
	int G36C::modulo_fire_reduce() {
		return modulo_fire_reduce_amount;
	}
	int G36C::bonus_burst() {
		return bonus_burst_amount;
	}
	int G36C::gun_damage() {
		return rng::between(dmg_lo(),dmg_hi()) + bonus_dmg();
	}
	timeline::interval_t G36C::ms_registration() const {
		return (timeline::interval_t)((*stats)[WPN_MS_REGISTRATION]);
	}

	int G36C::cooldown_between_shots() {
		return (*stats)[WPN_COOLDOWN_BETWEEN_SHOTS];
	}

	bool G36C::should_fire() {
		if(last_tick + cooldown_between_shots() <= tick::get()) {
			last_tick = tick::get();
			return true;
		}
		return false;
	}

	int G36C::burst() {
		return burst_base + bonus_burst();
	}

	auto G36C::weapon_stats() {
		return &data::g36c::stats;
	}

};
