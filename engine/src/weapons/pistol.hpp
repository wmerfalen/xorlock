#ifndef __WEAPONS_PISTOL_HEADER__
#define __WEAPONS_PISTOL_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "../weapons.hpp"
#include "../tick.hpp"
#include "../rng.hpp"
#include "../timeline.hpp"

namespace weapons {
  namespace pistol {
    /** [0] */ static constexpr uint32_t FLAGS = (uint32_t)(wpn::Flags::BURST_FIRE);
    static constexpr uint32_t WEAPON_TYPE = wpn::weapon_type_t::WPN_T_PISTOL;
    /** [1] */ static constexpr uint32_t GUN_DAMAGE_RANDOM_LO = 21;
    /** [2] */ static constexpr uint32_t GUN_DAMAGE_RANDOM_HI = 38;
    /** [3] */ static constexpr uint32_t BURST_DELAY_MS = 3;
    /** [4] */ static constexpr uint32_t PIXELS_PER_TICK = 30;
    /** [5] */ static constexpr uint32_t CLIP_SIZE = 9;
    /** [6] */ static constexpr uint32_t AMMO_MAX = CLIP_SIZE * 8;
    /** [7] */ static constexpr uint32_t RELOAD_TM = 1000;
    /** [8] */ static constexpr uint32_t COOLDOWN_BETWEEN_SHOTS = 120;
    /** [9] */ static constexpr uint32_t MS_REGISTRATION = (uint32_t)timeline::interval_t::MS_2;
    /** [10]*/ static constexpr uint32_t MAG_EJECT_TICKS = 350;
    /** [11]*/ static constexpr uint32_t PULL_REPLACEMENT_MAG_TICKS = 350;
    /** [12]*/ static constexpr uint32_t LOADING_MAG_TICKS = 350;
    /** [13]*/ static constexpr uint32_t SLIDE_PULL_TICKS = 350;
    /** [14]*/static constexpr uint32_t WEAPON_WIELD_TICKS = 350;
    /** [15]*/static constexpr uint32_t WEAPON_ACCURACY = 70;
    /** [16]*/static constexpr uint32_t WEAPON_ACCURACY_DEVIATION_START = 8;
    /** [17]*/static constexpr uint32_t WEAPON_ACCURACY_DEVIATION_END = 24;
    static weapon_stats_t stats = {
      FLAGS,
      WEAPON_TYPE,
      GUN_DAMAGE_RANDOM_LO,
      GUN_DAMAGE_RANDOM_HI,
      BURST_DELAY_MS,
      PIXELS_PER_TICK,
      CLIP_SIZE,
      AMMO_MAX,
      RELOAD_TM,
      COOLDOWN_BETWEEN_SHOTS,
      MS_REGISTRATION,
      MAG_EJECT_TICKS,
      PULL_REPLACEMENT_MAG_TICKS,
      LOADING_MAG_TICKS,
      SLIDE_PULL_TICKS,
      WEAPON_WIELD_TICKS,
      WEAPON_ACCURACY,
      WEAPON_ACCURACY_DEVIATION_START,
      WEAPON_ACCURACY_DEVIATION_END,
    };
  };
  struct Pistol {
    weapon_stats_t stats;
    int bonus_hi_dmg_amount;
    int bonus_lo_dmg_amount;
    int bonus_burst_amount;
    int bonus_dmg_amount;
    int modulo_fire_reduce_amount;
    uint64_t last_tick;
    uint64_t current_tick;
    uint16_t ammo;
    uint16_t total_ammo;
    Pistol();
    Pistol(const Pistol& other) = delete;
    const uint16_t& consume_ammo();
    int dmg_lo();
    int dmg_hi();
    int bonus_lo_dmg();
    int bonus_hi_dmg();
    int bonus_dmg();
    int modulo_fire_reduce();
    int bonus_burst();
    int gun_damage();
    timeline::interval_t ms_registration() const;
    void feed(const weapon_stats_t& in_stats);
    uint32_t stat(const uint32_t& st) const;

    int cooldown_between_shots();

    bool should_fire();

    int burst();

    weapon_stats_t* weapon_stats();
    uint16_t weapon_wield_ticks();

  };
};
#endif
