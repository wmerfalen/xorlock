#include <SDL2/SDL.h>
#include <iostream>
#include <array>

#include "pistol.hpp"
#include "../rng.hpp"

namespace weapons {
	Pistol::Pistol() :
		bonus_hi_dmg_amount(0),
		bonus_lo_dmg_amount(0),
		bonus_burst_amount(0),
		bonus_dmg_amount(0),
		modulo_fire_reduce_amount(0),
		last_tick(tick::get()) {
		ammo = 9;
		total_ammo = ammo * 10;
    stats[WPN_DMG_LO] = 10; // uint32_t GUN_DAMAGE_RANDOM_LO = 21;
    stats[WPN_DMG_HI] = 20; // uint32_t GUN_DAMAGE_RANDOM_HI = 38;
				stats[WPN_BURST_DLY] = 3;//uint32_t BURST_DELAY_MS = 3;
				stats[WPN_PIXELS_PT] = 30;//uint32_t PIXELS_PER_TICK = 30;
    stats[WPN_PIXELS_PT] = 15; 
				stats[WPN_CLIP_SZ] = 30; //uint32_t CLIP_SIZE = 30;
        stats[WPN_AMMO_MX] = stats[WPN_CLIP_SZ] * 8;//uint32_t AMMO_MAX = CLIP_SIZE * 8;
				stats[WPN_RELOAD_TM] = 1000;//uint32_t RELOAD_TM = 1000;
				stats[WPN_COOLDOWN_BETWEEN_SHOTS] = 120;//uint32_t COOLDOWN_BETWEEN_SHOTS = 120;
				stats[WPN_MS_REGISTRATION] = (uint32_t)timeline::interval_t::MS_2;
				stats[WPN_MAG_EJECT_TICKS] = 350;//uint32_t MAG_EJECT_TICKS = 350;
				stats[WPN_PULL_REPLACEMENT_MAG_TICKS] = 350;//uint32_t PULL_REPLACEMENT_MAG_TICKS = 350;
				stats[WPN_LOADING_MAG_TICKS] = 350;//uint32_t LOADING_MAG_TICKS = 350;
				stats[WPN_SLIDE_PULL_TICKS] = 350;//uint32_t SLIDE_PULL_TICKS = 350;
    stats[WPN_WIELD_TICKS] = 150;
	}
	const uint16_t& Pistol::consume_ammo() {
		if(ammo == 0) {
			return ammo;
		}
		--ammo;
		return ammo;
	}
	int Pistol::dmg_lo() {
		return stats[WPN_DMG_LO] + bonus_lo_dmg();
	}
	int Pistol::dmg_hi() {
		return stats[WPN_DMG_HI] + bonus_hi_dmg();
	}
	int Pistol::bonus_lo_dmg() {
		return rand_between(0,bonus_lo_dmg_amount);
	}
	int Pistol::bonus_hi_dmg() {
		return rand_between(0,bonus_hi_dmg_amount);
	}
	int Pistol::bonus_dmg() {
		return rand_between(0,bonus_dmg_amount);
	}
	int Pistol::modulo_fire_reduce() {
		return modulo_fire_reduce_amount;
	}
	int Pistol::bonus_burst() {
		return bonus_burst_amount;
	}
	int Pistol::gun_damage() {
		return rng::between(dmg_lo(),dmg_hi()) + bonus_dmg();
	}
	timeline::interval_t Pistol::ms_registration() const {
		return (timeline::interval_t)(stats[WPN_MS_REGISTRATION]);
	}

	int Pistol::cooldown_between_shots() {
    return stats[WPN_COOLDOWN_BETWEEN_SHOTS];
	}

	bool Pistol::should_fire() {
		if(last_tick + cooldown_between_shots() <= tick::get()) {
			last_tick = tick::get();
			return true;
		}
		return false;
	}

	int Pistol::burst() {
		return 3 + bonus_burst();
	}
  void Pistol::feed(const weapon_stats_t& in_stats){
    bcopy(&in_stats,&stats,sizeof(weapon_stats_t));
  }

	weapon_stats_t* Pistol::weapon_stats() {
		return &pistol::stats;
	}

  uint16_t Pistol::weapon_wield_ticks(){
    auto w = stats[WPN_WIELD_TICKS];
    if(rng::chance(10)){
      w /= 2;
    }
    return w;
  }
};
