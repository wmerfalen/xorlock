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
    (*stats)[WPN_DMG_LO] = 10; // uint32_t GUN_DAMAGE_RANDOM_LO = 21;
    (*stats)[WPN_DMG_HI] = 20; // uint32_t GUN_DAMAGE_RANDOM_HI = 38;
				//uint32_t BURST_DELAY_MS = 3;
				//uint32_t PIXELS_PER_TICK = 30;
    (*stats)[WPN_CLIP_SZ] = 9;
    (*stats)[WPN_PIXELS_PT] = 15; //uint32_t CLIP_SIZE = 30;
				//uint32_t AMMO_MAX = CLIP_SIZE * 8;
				//uint32_t RELOAD_TM = 1000;
				//uint32_t COOLDOWN_BETWEEN_SHOTS = 120;
				//uint32_t MS_REGISTRATION = (uint32_t)timeline::interval_t::MS_2;
				//uint32_t MAG_EJECT_TICKS = 350;
				//uint32_t PULL_REPLACEMENT_MAG_TICKS = 350;
				//uint32_t LOADING_MAG_TICKS = 350;
				//uint32_t SLIDE_PULL_TICKS = 350;
    (*stats)[WPN_WIELD_TICKS] = 150;
	}
	const uint16_t& P226::consume_ammo() {
		if(ammo == 0) {
			return ammo;
		}
		--ammo;
		return ammo;
	}
	int P226::dmg_lo() {
		return 10 + bonus_lo_dmg();
	}
	int P226::dmg_hi() {
		return 20 + bonus_hi_dmg();
	}
	int P226::bonus_lo_dmg() {
		return rand_between(0,bonus_lo_dmg_amount);
	}
	int P226::bonus_hi_dmg() {
		return rand_between(0,bonus_hi_dmg_amount);
	}
	int P226::bonus_dmg() {
		return rand_between(0,bonus_dmg_amount);
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
    return 423;
	}

	bool P226::should_fire() {
		if(last_tick + cooldown_between_shots() <= tick::get()) {
			last_tick = tick::get();
			return true;
		}
		return false;
	}

	int P226::burst() {
		return burst_base + bonus_burst();
	}

	weapon_stats_t* P226::weapon_stats() {
		return stats;
	}

  uint16_t P226::weapon_wield_ticks(){
    return 350;
  }
};
