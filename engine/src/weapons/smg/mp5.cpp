#include <SDL2/SDL.h>
#include <iostream>
#include <array>

#include "mp5.hpp"
#include "../../rng.hpp"

namespace weapons::smg {
  MP5::MP5() :
    bonus_hi_dmg_amount(0),
    bonus_lo_dmg_amount(0),
    bonus_burst_amount(0),
    bonus_dmg_amount(0),
    modulo_fire_reduce_amount(0),
    last_tick(tick::get()) {
      ammo = 35;
      total_ammo = ammo * 20;
      (*stats)[WPN_WIELD_TICKS] = 850;
    }
  const uint16_t& MP5::consume_ammo() {
    if(ammo == 0) {
      return ammo;
    }
    --ammo;
    return ammo;
  }
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
    return 130;
    return (*stats)[WPN_COOLDOWN_BETWEEN_SHOTS];
  }

  bool MP5::should_fire() {
    if(last_tick + cooldown_between_shots() <= tick::get()) {
      last_tick = tick::get();
      //std::cout << ".\n";
      return true;
    }
    return false;
  }

  int MP5::burst() {
    return burst_base + bonus_burst();
  }

  weapon_stats_t* MP5::weapon_stats() {
    return &data::mp5::stats;
  }
  uint16_t MP5::weapon_wield_ticks(){
    return 1250;
  }

};
