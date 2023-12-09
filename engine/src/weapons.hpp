#ifndef __WEAPONS_HEADER__
#define __WEAPONS_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <array>

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
  };
	enum weapon_t {
		WPN_MP5 = 0,
		WPN_AR15,
		WPN_UMP45,
		WPN_G36C,
		WPN_COMMANDO_512,
		WPN_AUG_PARA,
		WPN_AUG_A3,
		WPN_FAMAS,
    WPN_P226,
    WPN_FRAG,
    WPN_MAX_SIZE = WPN_FRAG + 1,
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
  __WPN_SIZE = WPN_WIELD_TICKS + 1,
};
enum EXPLOSIVE : uint32_t {
  EXP_FLAGS = 0,
  EXP_DMG_LO,
  EXP_DMG_HI,
  EXP_PULL_PIN_TICKS,
  EXP_RADIUS,
  __EXPLOSIVE_SIZE = EXP_RADIUS + 1,
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
#endif
