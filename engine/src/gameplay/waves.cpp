#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include "waves.hpp"
#include "../rng.hpp"

namespace gameplay {
	namespace waves {
		session::session(
		    uint16_t _in_wave_cnt,
		    uint16_t _base_wave_npc_cnt,
		    float _increase_per_wave,
		    bool _be_chaotic) :
			current_wave(0),
			wave_count(_in_wave_cnt),
			base_wave_npc_count(_base_wave_npc_cnt),
			increase_per_wave(_increase_per_wave),
			be_chaotic(_be_chaotic) {
			roll_wave();
		}
		void session::roll_wave() {
			npcs_per_wave.clear();
			current_wave = 0;
			for(unsigned i=0; i < wave_count; i++) {
				uint16_t npcs_this_wave = base_wave_npc_count;
				if(i) {
					npcs_this_wave += increase_per_wave * base_wave_npc_count;
					if(be_chaotic) {
						npcs_this_wave += rand_between(1,i + 1 * wave_count);
					}
				}
				npcs_per_wave.emplace_back(npcs_per_wave);
			}
		}
		uint16_t session::remaining_waves() {
			return npcs_per_wave.size() - (current_wave + 1);
		}
		uint16_t session::get_next_wave_count() {
			if(current_wave < npcs_per_wave.size()) {
				return npcs_per_wave[current_wave];
			}
			return 0;
		}
		void session::next_wave() {
			current_wave += 1;
		}

		void init() {

		}
	};
};
