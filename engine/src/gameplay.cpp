#include <SDL2/SDL.h>
#include <iostream>
#include "gameplay.hpp"
#include "font.hpp"
#include "player.hpp"
#include "gameplay/waves.hpp"

namespace gameplay {
	static constexpr const char* SK_BABY = "I'm too young to die";
	static constexpr const char* SK_EASY = "Hey, not too rough";
	static constexpr const char* SK_MEDIUM = "Hurt me plenty";
	static constexpr const char* SK_HARD = "Ultra-Violence";
	static constexpr const char* SK_NIGHTMARE = "Nightmare!";
	struct current_game {
		std::unique_ptr<waves::session> session;
		bool game_is_over;
		current_game() = delete;
		current_game(const std::string difficulty) {
			game_is_over = false;
			uint16_t in_wave_cnt = 10;
			uint16_t base_wave_npc_cnt = 10;
			float increase_per_wave = 2.0;
			bool be_chaotic = false;
			if(difficulty.compare(SK_BABY) == 0) {
				in_wave_cnt = 5;
				base_wave_npc_cnt = 5;
				increase_per_wave = 2.0;
				be_chaotic = false;
			}
			if(difficulty.compare(SK_EASY) == 0) {
				in_wave_cnt = 5;
				base_wave_npc_cnt = 8;
				increase_per_wave = 2.0;
				be_chaotic = false;
			}
			if(difficulty.compare(SK_MEDIUM) == 0) {
				in_wave_cnt = 5;
				base_wave_npc_cnt = 12;
				increase_per_wave = 2.0;
				be_chaotic = false;
			}
			if(difficulty.compare(SK_HARD) == 0) {
				in_wave_cnt = 8;
				base_wave_npc_cnt = 20;
				increase_per_wave = 2.0;
				be_chaotic = false;
			}
			if(difficulty.compare(SK_NIGHTMARE) == 0) {
				in_wave_cnt = 15;
				base_wave_npc_cnt = 35;
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

		void start_wave() {
			spawn(session->get_wave_count());
		}
		void game_over(bool is_over) {
			game_is_over = is_over;
		}
		void spawn(const uint16_t& count) {
			if(count == 0) {
				game_over(true);
				return;
			}
			for(uint16_t i=0; i < count; i++) {
				npc::spawn_spetsnaz((1024 / 2) + (i * CELL_WIDTH), (1024 / 2) - (i * CELL_HEIGHT));
			}
		}
		void next_wave() {
			session->next_wave();
		}
		bool over() const {
			return game_is_over;
		}
	};
	static std::unique_ptr<current_game> game;
	static tick_t game_start_tick;
	static SDL_Point wave_message;
	void init() {
		game_start_tick = tick::get();
		npc_spawning::init();
		game = std::make_unique<current_game>(SK_BABY);
	}
	void tick() {
		static bool started_game=false;
		static std::string msg = "Wave starting soon...";
		static tick_t started = 0;
		wave_message.x = plr::cx() - 550;
		wave_message.y = plr::cy() - 250;
		if(game->over()) {
			font::green_text(&wave_message,"Game over",50,900);
			return;
		}
		if(started_game == false) {
			if(game_start_tick + 2500 < tick::get()) {
				game->start_wave();
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
		if(started_game && npc::alive_count() == 0) {
			if(!game->over()) {
				npc::cleanup_corpses();
				font::green_text(&wave_message,"Incoming wave...",50,900);
				game_start_tick = tick::get() + 2500;
				started_game = false;
				game->next_wave();
			}
		}
	}
}; // end namespace gameplay
