#include <SDL2/SDL.h>
#include <iostream>
#include <array>

#include "p226.hpp"
#include "../../rng.hpp"

namespace weapons::pistol {
	P226::P226() :
		bonus_hi_dmg_amount(0),
		bonus_lo_dmg_amount(0),
		bonus_burst_amount(0),
		bonus_dmg_amount(0),
		modulo_fire_reduce_amount(0),
		last_tick(tick::get()) {
		ammo = 9;
		total_ammo = ammo * 10;
	}
	const uint16_t& P226::consume_ammo() {
		if(ammo == 0) {
			return ammo;
		}
		--ammo;
		return ammo;
	}
	int P226::dmg_lo() {
		return (*stats)[WPN_DMG_LO] + bonus_lo_dmg();
	}
	int P226::dmg_hi() {
		return (*stats)[WPN_DMG_HI] + bonus_hi_dmg();
	}
	int P226::bonus_lo_dmg() {
		return bonus_lo_dmg_amount;
	}
	int P226::bonus_hi_dmg() {
		return bonus_hi_dmg_amount;
	}
	int P226::bonus_dmg() {
		return bonus_dmg_amount;
	}
	int P226::modulo_fire_reduce() {
		return modulo_fire_reduce_amount;
	}
	int P226::bonus_burst() {
		return bonus_burst_amount;
	}
	int P226::gun_damage() {
		return rng::between(dmg_lo(),dmg_hi()) + bonus_dmg();
	}
	timeline::interval_t P226::ms_registration() const {
		return (timeline::interval_t)((*stats)[WPN_MS_REGISTRATION]);
	}

	int P226::cooldown_between_shots() {
    return 530;
	}

	bool P226::should_fire() {
		if(last_tick + cooldown_between_shots() <= tick::get()) {
			last_tick = tick::get();
      //std::cout << ".\n";
			return true;
		}
		return false;
	}

	int P226::burst() {
		return burst_base + bonus_burst();
	}

	auto P226::weapon_stats() {
		return &data::p226::stats;
	}

};
