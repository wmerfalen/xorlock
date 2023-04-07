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
};
enum WPN {
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
};
using weapon_stats_t = std::array<uint32_t,10>;

#endif
