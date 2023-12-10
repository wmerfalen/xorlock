#include <SDL2/SDL.h>
#include <iostream>
#include <array>

#include "frag.hpp"
#include "../../rng.hpp"

namespace weapons::grenade {
	Frag::Frag() :
		bonus_hi_dmg_amount(0),
		bonus_lo_dmg_amount(0) {
    (*stats)[EXP_DMG_LO] = 10; // uint32_t GUN_DAMAGE_RANDOM_LO = 21;
    (*stats)[EXP_DMG_HI] = 20; // uint32_t GUN_DAMAGE_RANDOM_HI = 38;
    (*stats)[EXP_PULL_PIN_TICKS] = 15; //uint32_t CLIP_SIZE = 30;
    (*stats)[EXP_RADIUS] = 45; //uint32_t CLIP_SIZE = 30;
    ammo = 3;
    total_ammo = 3;
	}
	int Frag::dmg_lo() {
		return 10 + bonus_lo_dmg();
	}
	int Frag::dmg_hi() {
		return 20 + bonus_hi_dmg();
	}
	int Frag::bonus_lo_dmg() {
		return rand_between(0,bonus_lo_dmg_amount);
	}
	int Frag::bonus_hi_dmg() {
		return rand_between(0,bonus_hi_dmg_amount);
	}

	explosive_stats_t* Frag::explosive_stats() {
		return &data::frag::stats;
	}

};
