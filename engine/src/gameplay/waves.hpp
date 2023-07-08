#ifndef __GAMEPLAY_WAVES_HEADER__
#define __GAMEPLAY_WAVES_HEADER__
#include <iostream>
#include <vector>

namespace gameplay {
	namespace waves {
		struct session {
				session() = delete;
				session(
				    uint16_t _in_wave_cnt,
				    uint16_t _base_wave_npc_cnt,
				    float _increase_per_wave,
				    bool _be_chaotic);
				void roll_wave();
				uint16_t remaining_waves();
				uint16_t get_next_wave_count();
				void next_wave();

			private:
				uint16_t current_wave;
				uint16_t wave_count;
				uint16_t base_wave_npc_count;
				float increase_per_wave;
				bool be_chaotic;
				std::vector<uint16_t> npcs_per_wave;
		};
		void init();
	};
};

#endif
