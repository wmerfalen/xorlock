#include <SDL2/SDL.h>
#include <iostream>
#include "gameplay.hpp"
#include "font.hpp"
#include "player.hpp"
#include "gameplay/waves.hpp"

namespace gameplay {
	struct current_game {
		std::unique_ptr<waves::session> session;
		current_game() = delete;
		current_game(const std::string difficulty) {
			uint16_t in_wave_cnt = 3;
			uint16_t base_wave_npc_cnt = 10;
			float increase_per_wave = 2.0;
			bool be_chaotic = false;
			if(difficulty.compare("EASY") == 0) {
				in_wave_cnt = 3;
				base_wave_npc_cnt = 5;
				increase_per_wave = 2.0;
				be_chaotic = false;
			}
			session = std::make_unique<waves::session>(
			              in_wave_cnt,
			              base_wave_npc_cnt,
			              increase_per_wave,
			              be_chaotic
			          );
		}

		void start_game() {
			auto count = session->get_wave_count();
			for(auto i=0; i < count; i++) {
				npc::spawn_spetsnaz((1024 / 2) + (i * CELL_WIDTH), (1024 / 2) - (i * CELL_HEIGHT));
			}
		}
	};
	static std::unique_ptr<current_game> game;
	static tick_t game_start_tick;
	static SDL_Point wave_message;
	void init() {
		game_start_tick = tick::get();
		npc_spawning::init();
		game = std::make_unique<current_game>("EASY");
	}
	void tick() {
		static bool started_game=false;
		static std::string msg = "Wave starting soon...";
		static tick_t started = 0;
		wave_message.x = plr::cx() - 550;
		wave_message.y = plr::cy() - 250;
		if(started_game == false) {
			if(game_start_tick + 2500 < tick::get()) {
				game->start_game();
				started_game = true;
				started = tick::get();
			} else {
				font::green_text(&wave_message,msg,50,900);
			}
		} else {
			if(started + 3500 > tick::get()) {
				wave_message.x = plr::cx();
				font::green_text(&wave_message,"go!",50,100);
			}
		}
	}
}; // end namespace gameplay
