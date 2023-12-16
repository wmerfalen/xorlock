#ifndef __WEAPONS_GRENADE_FRAG_HEADER__
#define __WEAPONS_GRENADE_FRAG_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "../../weapons.hpp"
#include "../../tick.hpp"
#include "../../rng.hpp"
#include "../../timeline.hpp"

namespace weapons {
	namespace grenade {
		namespace data {
			namespace frag {
				/** [0] */ static constexpr uint32_t FLAGS = 0;
        /** [1] */ static constexpr uint32_t TYPE = (uint32_t)wpn::grenade_type_t::GREN_T_FRAG;
				/** [2] */ static constexpr uint32_t DAMAGE_RANDOM_LO = 21;
				/** [3] */ static constexpr uint32_t DAMAGE_RANDOM_HI = 38;
				/** [4] */ static constexpr uint32_t PULL_PIN_TICKS = 200;
				/** [5] */ static constexpr uint32_t EXPLOSIVE_RADIUS = 200;
				static explosive_stats_t stats = {
					FLAGS,
          TYPE,
					DAMAGE_RANDOM_LO,
					DAMAGE_RANDOM_HI,
          PULL_PIN_TICKS,
          EXPLOSIVE_RADIUS,
				};
			};
		};
		struct Frag {
			explosive_stats_t * stats = &data::frag::stats;
			int bonus_hi_dmg_amount;
			int bonus_lo_dmg_amount;
			Frag();
			Frag(const Frag& other) = delete;
			int dmg_lo();
			int dmg_hi();
			int bonus_lo_dmg();
			int bonus_hi_dmg();

      uint16_t ammo;
      uint16_t total_ammo;
			explosive_stats_t* explosive_stats();
		};
	};

};
#endif
