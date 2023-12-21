#ifndef __WEAPONS_HEADER__
#define __WEAPONS_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <array>
#include "rng.hpp"

namespace wpn {
  enum Flags : uint32_t {
    SEMI_AUTOMATIC = (1 << 0),
    BURST_FIRE = (1 << 1),
    AUTOMATIC = (1 << 2),
    SEMI_AUTOMATIC_SHOTGUN = (1 << 3),
    AUTOMATIC_SHOTGUN = (1 << 4),
    PROJECTILE_LAUNCHER = (1 << 5),
  };
  enum weapon_type_t : uint16_t {
    WPN_T_AR = 0,
    WPN_T_SMG = 1,
    WPN_T_SHOTGUN = 2,
    WPN_T_PISTOL = 3,
    WPN_T_MACHINE_PISTOL = 4,
    WPN_T_SNIPER = 5,
    WPN_T_LMG = 6,
    WPN_T_DMR = 7,
    _WPN_T_MAX = WPN_T_DMR + 1,
  };
  enum grenade_type_t : uint16_t {
    GREN_T_FRAG,
    GREN_T_STUN,
    GREN_T_SMOKE,
    GREN_T_INCENDIARY,
    _GREN_T_MAX = GREN_T_INCENDIARY + 1,
  };
  static std::array<std::string,_WPN_T_MAX> weapon_strings = {
    "assault-rifle",
    "smg",
    "shotgun",
    "pistol",
    "machine-pistol",
    "sniper",
    "lmg",
    "dmr",
  };
  static std::array<std::string,_GREN_T_MAX> grenade_strings = {
    "frag",
    "stun",
    "smoke",
    "incendiary",
  };
  static inline std::string to_string(grenade_type_t& t){
    return grenade_strings[t];
  }
  static inline std::string to_string(weapon_type_t& t){
    return weapon_strings[t];
  }
  enum weapon_t : uint32_t {
    WPN_MP5 = 0,
    WPN_AR15,
    WPN_UMP45,
    WPN_G36C,
    WPN_COMMANDO_512,
    WPN_AUG_PARA,
    WPN_AUG_A3,
    WPN_FAMAS,
    WPN_P226,
    WPN_GLOCK,
    WPN_FRAG,
    WPN_92FS, // PISTOL
    WPN_P99, // PISTOL
    WPN_DESERT_EAGLE,
    WPN_GLOCK_18,
    WPN_USP, // PISTOL
    WPN_MK23, // PISTOL
    WPN_G3KA4,
    WPN_TAR21,
    WPN_SPAS12,
    WPN_INCENDIARY_GRENADE,
    WPN_P90,
    __META_WEAPON_T_LAST,
    WPN_MAX_SIZE = __META_WEAPON_T_LAST,
  };
  enum position_t {
    POS_PRIMARY,
    POS_SECONDARY,
  };
};
enum WPN : uint32_t {
  WPN_FLAGS = 0,
  WPN_TYPE,
  WPN_DMG_LO,
  WPN_DMG_HI,
  WPN_BURST_DLY,
  WPN_PIXELS_PT,
  WPN_CLIP_SZ,
  WPN_AMMO_MX,
  WPN_RELOAD_TM,
  WPN_COOLDOWN_BETWEEN_SHOTS,
  WPN_MS_REGISTRATION,
  WPN_MAG_EJECT_TICKS,
  WPN_PULL_REPLACEMENT_MAG_TICKS,
  WPN_LOADING_MAG_TICKS,
  WPN_SLIDE_PULL_TICKS,
  WPN_WIELD_TICKS,
  WPN_ACCURACY,
  WPN_ACCURACY_DEVIATION_START,
  WPN_ACCURACY_DEVIATION_END,
  __META_WPN_LAST,
  __WPN_SIZE = __META_WPN_LAST,
};
static constexpr std::array<std::string_view,WPN::__WPN_SIZE> weapon_slot_strings = {
  "flags",
  "type",
  "damage_low",
  "damage_high",
  "burst_delay",
  "pixels_per_tick",
  "clip_size",
  "ammo_max",
  "reload_time",
  "cooldown_between_shots",
  "ms_registration",
  "mag_eject_ticks",
  "pull_replacement_ticks",
  "loading_mag_ticks",
  "wield_ticks",
  "accuracy",
  "acc_deviation_start",
  "acc_deviation_end",
};
static constexpr std::array<std::string_view,__WPN_SIZE> user_friendly_weapon_slot_strings= {
  "flags",
  "type",
  "damage low",
  "damage high",
  "burst delay",
  "velocity",
  "clip size",
  "max ammo",
  "reload time",
  "rate of fire",
  "ms reg",
  "eject time",
  "pull mag time",
  "mag load time",
  "slide pull time",
  "wield time",
  "accuracy",
  "acc_start",
  "acc_end",
};
enum EXPLOSIVE : uint32_t {
  EXP_FLAGS = 0,
  EXP_TYPE,
  EXP_DMG_LO,
  EXP_DMG_HI,
  EXP_PULL_PIN_TICKS,
  EXP_RADIUS,
  __EXPLOSIVE_SIZE = EXP_RADIUS + 1,
};
static constexpr std::array<std::string_view,__EXPLOSIVE_SIZE> explosive_slot_strings = {
  "flags",//EXP_FLAGS = 0,
  "type",//EXP_TYPE,
  "dmg_lo",//EXP_DMG_LO,
  "dmg_hi",//EXP_DMG_HI,
  "pull_pin_ticks",//EXP_PULL_PIN_TICKS,
  "radius",//EXP_RADIUS,
};
static constexpr std::array<std::string_view,__EXPLOSIVE_SIZE> user_friendly_explosive_slot_strings= {
  "flags",
  "type",
  "damage low",
  "damage high",
  "pull pin time",
  "radius",
};
using weapon_stats_t = std::array<uint32_t,__WPN_SIZE>;
using explosive_stats_t = std::array<uint32_t,__EXPLOSIVE_SIZE>;
struct weapon_instance_t {
  weapon_stats_t stats;
  bool should_fire();
  std::string name;
  std::array<Mix_Chunk*,10> sounds;
  uint64_t last_fire_tick;
  weapon_instance_t() = delete;
};
static constexpr std::size_t PISTOL_MAX = 8;
static constexpr std::size_t EXPLOSIVE_MAX = 2;
static constexpr std::array<wpn::weapon_t,EXPLOSIVE_MAX> explosive_types = {
  wpn::weapon_t::WPN_FRAG,
  wpn::weapon_t::WPN_INCENDIARY_GRENADE,
};
static constexpr std::array<wpn::weapon_t,PISTOL_MAX> pistol_types = {
  wpn::weapon_t::WPN_P226,
  wpn::weapon_t::WPN_GLOCK,
  wpn::weapon_t::WPN_92FS, // PISTOL
  wpn::weapon_t::WPN_P99, // PISTOL
  wpn::weapon_t::WPN_DESERT_EAGLE,
  wpn::weapon_t::WPN_GLOCK_18,
  wpn::weapon_t::WPN_USP, // PISTOL
  wpn::weapon_t::WPN_MK23, // PISTOL
};
static constexpr std::size_t SMG_MAX = 3;
static constexpr std::array<wpn::weapon_t,SMG_MAX> smg_types = {
  wpn::weapon_t::WPN_MP5,
  wpn::weapon_t::WPN_UMP45,
  wpn::weapon_t::WPN_P90,
};

static inline bool is_secondary(uint32_t t){
  return std::find(pistol_types.cbegin(),pistol_types.cend(),t) != pistol_types.cend();
}
static inline bool is_explosive(uint32_t t){
  return std::find(explosive_types.cbegin(),explosive_types.cend(),t) != explosive_types.cend();
}
static inline bool is_primary(uint32_t t){
  return !is_secondary(t) && !is_explosive(t);
}

extern int rand_between(const int& min,const int& max);
static inline wpn::weapon_t random_pistol_type(){
  return pistol_types[rand_between(1,100) % PISTOL_MAX];
}
static inline std::string weapon_name(weapon_stats_t* w){
  switch((wpn::weapon_t)(*w)[WPN_TYPE]){
    case wpn::weapon_t::WPN_92FS: return "92FS";
    case wpn::weapon_t::WPN_P99: return "P99";
    case wpn::weapon_t::WPN_DESERT_EAGLE: return "Desert Eagle";
    case wpn::weapon_t::WPN_GLOCK_18: return "GLOCK 18";
    case wpn::weapon_t::WPN_USP: return "USP";
    case wpn::weapon_t::WPN_MK23: return "MK23";
    case wpn::weapon_t::WPN_MP5: return "MP5";
    case wpn::weapon_t::WPN_AR15: return "AR15";
    case wpn::weapon_t::WPN_UMP45: return "UMP-45";
    case wpn::weapon_t::WPN_G36C: return "G36C";
    case wpn::weapon_t::WPN_COMMANDO_512: return "Commando 512";
    case wpn::weapon_t::WPN_AUG_PARA: return "AUG PARA";
    case wpn::weapon_t::WPN_AUG_A3: return "AUG A3";
    case wpn::weapon_t::WPN_FAMAS: return "FAMAS";
    case wpn::weapon_t::WPN_P226: return "P226";
    case wpn::weapon_t::WPN_GLOCK: return "GLOCK";
    case wpn::weapon_t::WPN_FRAG: return "FRAG";
    case wpn::weapon_t::WPN_SPAS12: return "SPAS-12";
    case wpn::weapon_t::WPN_P90: return "P90";
    default:
                                    return "";
  }
}
static inline bool is_smg(uint32_t w){
  return std::find(smg_types.cbegin(),smg_types.cend(),w) != smg_types.cend();
}

static inline bool is_shotgun(uint32_t w){
  switch(w){
    case wpn::weapon_t::WPN_SPAS12:
      return true;
    default:
      return false;
  }
}
static inline bool is_pistol(uint32_t w){
  switch(w){
    case wpn::weapon_t::WPN_P226:
    case wpn::weapon_t::WPN_GLOCK:
    case wpn::weapon_t::WPN_92FS:
    case wpn::weapon_t::WPN_P99:
    case wpn::weapon_t::WPN_DESERT_EAGLE:
    case wpn::weapon_t::WPN_GLOCK_18:
    case wpn::weapon_t::WPN_USP:
    case wpn::weapon_t::WPN_MK23:
      return true;
    default:
      return false;
  }
}
namespace wpn_info {
  static std::array<bool,__WPN_SIZE> skip = {
    true,//WPN_FLAGS,
    true,//WPN_TYPE,
    false,//WPN_DMG_LO,
    false,//WPN_DMG_HI,
    false,//WPN_BURST_DLY,
    false,//WPN_PIXELS_PT,
    false,//WPN_CLIP_SZ,
    false,//WPN_AMMO_MX,
    false,//WPN_RELOAD_TM,
    false,//WPN_COOLDOWN_BETWEEN_SHOTS,
    true,//WPN_MS_REGISTRATION,
    false,//WPN_MAG_EJECT_TICKS,
    false,//WPN_PULL_REPLACEMENT_MAG_TICKS,
    false,//WPN_LOADING_MAG_TICKS,
    false,//WPN_SLIDE_PULL_TICKS,
    false,//WPN_WIELD_TICKS,
    false,//WPN_ACCURACY
    false,//WPN_ACCURACY_DEVIATION_START
    false,//WPN_ACCURACY_DEVIATION_END
  };
  static inline std::vector<std::string> weapon_stats(weapon_stats_t * s){
    std::vector<std::string> page;
    for(const auto& field : {
        WPN_FLAGS,
        WPN_TYPE,
        WPN_DMG_LO,
        WPN_DMG_HI,
        WPN_BURST_DLY,
        WPN_PIXELS_PT,
        WPN_CLIP_SZ,
        WPN_AMMO_MX,
        WPN_RELOAD_TM,
        WPN_COOLDOWN_BETWEEN_SHOTS,
        WPN_MS_REGISTRATION,
        WPN_MAG_EJECT_TICKS,
        WPN_PULL_REPLACEMENT_MAG_TICKS,
        WPN_LOADING_MAG_TICKS,
        WPN_SLIDE_PULL_TICKS,
        WPN_WIELD_TICKS,
        WPN_ACCURACY,
        WPN_ACCURACY_DEVIATION_START,
        WPN_ACCURACY_DEVIATION_END,
        }){
      if(skip[field]){
        continue;
      }
      page.emplace_back(std::string(user_friendly_weapon_slot_strings[field]) + std::string(": ") + std::to_string((*s)[field]));
    }
    return page;
  }
  static std::array<bool,__EXPLOSIVE_SIZE> skip_grenade = {
    true,//EXP_FLAGS,
    false,// EXP_TYPE
    false,//EXP_DMG_LO,
    false,//EXP_DMG_HI,
    false,//EXP_PULL_PIN_TICKS,
  };

  static inline std::vector<std::string> explosive_stats(explosive_stats_t* s){
    std::vector<std::string> page;
    for(const auto& field : {
        EXP_FLAGS,
        EXP_TYPE,
        EXP_DMG_LO,
        EXP_DMG_HI,
        EXP_PULL_PIN_TICKS,
        EXP_RADIUS,
        }){
      if(skip_grenade[field]){
        continue;
      }
      page.emplace_back(std::string(user_friendly_explosive_slot_strings[field]) + std::string(": ") + std::to_string((*s)[field]));
    }
    return page;
  }
};
namespace wpn_debug {
  static inline void dump(weapon_stats_t * s){
    std::cout << "WEAPON DUMP>>>:\n";
    for(const auto& field : {
        WPN_FLAGS,
        WPN_TYPE,
        WPN_DMG_LO,
        WPN_DMG_HI,
        WPN_BURST_DLY,
        WPN_PIXELS_PT,
        WPN_CLIP_SZ,
        WPN_AMMO_MX,
        WPN_RELOAD_TM,
        WPN_COOLDOWN_BETWEEN_SHOTS,
        WPN_MS_REGISTRATION,
        WPN_MAG_EJECT_TICKS,
        WPN_PULL_REPLACEMENT_MAG_TICKS,
        WPN_LOADING_MAG_TICKS,
        WPN_SLIDE_PULL_TICKS,
        WPN_WIELD_TICKS,
        WPN_ACCURACY,
        WPN_ACCURACY_DEVIATION_START,
        WPN_ACCURACY_DEVIATION_END,
        }){
      std::cout << weapon_slot_strings[field] << ": " << (*s)[field] << "\n";
    }
    std::cout << "<<< END WEAPON DUMP\n";
  }
  static inline void dump(explosive_stats_t * s){
    std::cout << "FRAG DUMP>>>:\n";
    for(const auto& field : {
        EXP_FLAGS,
        EXP_TYPE,
        EXP_DMG_LO,
        EXP_DMG_HI,
        EXP_PULL_PIN_TICKS,
        EXP_RADIUS,
        }){
      std::cout << explosive_slot_strings[field] << ": " << (*s)[field] << "\n";
    }
    std::cout << "<<< END FRAG DUMP\n";
  }
};
#endif

