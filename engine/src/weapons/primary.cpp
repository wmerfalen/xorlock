#include <SDL2/SDL.h>
#include <iostream>
#include <array>

#include "primary.hpp"
#include "../rng.hpp"
#include "../weapons.hpp"

namespace weapons {
  Primary::Primary() :
    bonus_hi_dmg_amount(0),
    bonus_lo_dmg_amount(0),
    bonus_burst_amount(0),
    bonus_dmg_amount(0),
    modulo_fire_reduce_amount(0),
    last_tick(tick::get()) {
      ammo = 35;
      total_ammo = ammo * 20;
      stats = &actual_stats;
      actual_stats[WPN_TYPE] = wpn::weapon_t::WPN_MP5;
      actual_stats[WPN_WIELD_TICKS] = 850;
    }
  const uint16_t& Primary::consume_ammo() {
    if(ammo == 0) {
      return ammo;
    }
    --ammo;
    return ammo;
  }
  int Primary::dmg_lo() {
    return actual_stats[WPN_DMG_LO] + bonus_lo_dmg();
  }
  int Primary::dmg_hi() {
    return actual_stats[WPN_DMG_HI] + bonus_hi_dmg();
  }
  int Primary::bonus_lo_dmg() {
    return bonus_lo_dmg_amount;
  }
  int Primary::bonus_hi_dmg() {
    return bonus_hi_dmg_amount;
  }
  int Primary::bonus_dmg() {
    return bonus_dmg_amount;
  }
  int Primary::modulo_fire_reduce() {
    return modulo_fire_reduce_amount;
  }
  int Primary::bonus_burst() {
    return bonus_burst_amount;
  }
  int Primary::gun_damage() {
    return rng::between(dmg_lo(),dmg_hi()) + bonus_dmg();
  }
  timeline::interval_t Primary::ms_registration() const {
    return (timeline::interval_t)(actual_stats[WPN_MS_REGISTRATION]);
  }

  int Primary::cooldown_between_shots() {
    return actual_stats[WPN_COOLDOWN_BETWEEN_SHOTS];
  }

  int Primary::burst() {
    return burst_base + bonus_burst();
  }

  weapon_stats_t* Primary::weapon_stats() {
    return stats;
  }
  uint16_t Primary::weapon_wield_ticks(){
    return actual_stats[WPN_WIELD_TICKS];
  }

  const uint32_t& Primary::stat(const uint32_t& s) const {
    return actual_stats[s];
  }
  uint16_t* Primary::ammo_ptr(){
    return &ammo;
  }
  uint16_t* Primary::total_ammo_ptr(){
    return &total_ammo;
  }
  void Primary::feed(weapon_stats_t* p){
    if(!p){
      bcopy(&primary_data::stats,&actual_stats,sizeof(weapon_stats_t));
    }else{
      bcopy(p,&actual_stats,sizeof(weapon_stats_t));
    }
    m_weapon_name = ::weapon_name(&actual_stats);
  }
  std::string_view Primary::weapon_name() const{
    return m_weapon_name;
  }
};
