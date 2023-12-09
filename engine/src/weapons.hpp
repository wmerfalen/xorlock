#ifndef __WEAPONS_HEADER__
#define __WEAPONS_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
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
	enum weapon_t {
		WPN_MP5,
		WPN_AR15,
		WPN_UMP45,
		WPN_G36C,
		WPN_COMMANDO_512,
		WPN_AUG_PARA,
		WPN_AUG_A3,
		WPN_FAMAS,
    WPN_P226,
    WPN_FRAG,
	};
	enum position_t {
		POS_PRIMARY,
		POS_SECONDARY,
	};
};
enum WPN : uint32_t {
	WPN_FLAGS = 0,
	WPN_DMG_LO = 1,
	WPN_DMG_HI = 2,
	WPN_BURST_DLY = 3,
	WPN_PIXELS_PT = 4,
	WPN_CLIP_SZ = 5,
	WPN_AMMO_MX = 6,
	WPN_RELOAD_TM = 7,
	WPN_COOLDOWN_BETWEEN_SHOTS = 8,
	WPN_MS_REGISTRATION = 9,
  WPN_MAG_EJECT_TICKS = 10,
  WPN_PULL_REPLACEMENT_MAG_TICKS = 11,
  WPN_LOADING_MAG_TICKS = 12,
  WPN_SLIDE_PULL_TICKS = 13,
  __WPN_SIZE = WPN_SLIDE_PULL_TICKS + 1,
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
#endif
