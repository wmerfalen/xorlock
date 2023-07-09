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
				in_wave_cnt = 2;
				base_wave_npc_cnt = 5;
				increase_per_wave = 2.0;
				be_chaotic = false;
			}
			if(difficulty.compare(SK_EASY) == 0) {
				in_wave_cnt = 3;
				base_wave_npc_cnt = 8;
				increase_per_wave = 2.0;
				be_chaotic = false;
			}
			if(difficulty.compare(SK_MEDIUM) == 0) {
				in_wave_cnt = 4;
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
		void spawn(const uint16_t& count) {
			if(count == 0) {
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
			return session->get_wave_count() == 0;
		}
	};
	static std::unique_ptr<current_game> game;
	static tick_t game_start_tick;
	static SDL_Point wave_message;
	enum gamestate_t : uint16_t {
		GS_NOT_STARTED,
		GS_CHOOSE_DIFFICULTY,
		GS_INCOMING_WAVE,
		GS_WAVE_ACTIVE,
		GS_WAVE_COMPLETE,
		GS_WAVE_REWARDS,
		GS_WAVES_DONE,
		GS_CHOOSE_NEXT_AREA,
	};
	static gamestate_t game_state;
	void display_difficulty_prompt() {
		static SDL_Point msg_placement;

		msg_placement.x = plr::cx() - 520;
		msg_placement.y = plr::cy() - 520;
		font::green_text(&msg_placement,"1: baby",25,300);
		msg_placement.x = plr::cx() - 520;
		msg_placement.y = plr::cy() - 520 + 25;
		font::green_text(&msg_placement,"2: easy",25,300);
		msg_placement.x = plr::cx() - 520;
		msg_placement.y = plr::cy() - 520 + 25 * 2;
		font::green_text(&msg_placement,"3: medium",25,300);
		msg_placement.x = plr::cx() - 520;
		msg_placement.y = plr::cy() - 520 + 25 * 3;
		font::green_text(&msg_placement,"4: hard",25,300);
		msg_placement.x = plr::cx() - 520;
		msg_placement.y = plr::cy() - 520 + 25 * 4;
		font::green_text(&msg_placement,"5: nightmare",25,300);
	}
	void init() {
		game_start_tick = tick::get();
		npc_spawning::init();
		game_state = GS_CHOOSE_DIFFICULTY;
		std::cout << "gameplay init()\n";
	}
	static tick_t start_game_tick;
	void choose_difficulty(std::string choice) {
		game = std::make_unique<current_game>(choice);
		start_game_tick = tick::get() + 5000;
		game_state = GS_INCOMING_WAVE;
	}
	void numeric_pressed(uint8_t value) {
		if(game_state == GS_CHOOSE_DIFFICULTY) {
			std::cout << "GS_CHOOSE_DIFFICULTY\n";
			switch(value) {
				case 1:
					choose_difficulty(SK_BABY);
					break;
				case 2:
					choose_difficulty(SK_EASY);
					break;
				case 3:
					choose_difficulty(SK_MEDIUM);
					break;
				case 4:
					choose_difficulty(SK_HARD);
					break;
				case 5:
					choose_difficulty(SK_NIGHTMARE);
					break;
				default:
					break;
			}
		}
	}
	bool needs_numeric() {
		return game_state == GS_CHOOSE_DIFFICULTY;
	}
	static tick_t timer_tick;
	void tick() {
		static std::string msg_incoming_wave = "Incoming wave...";
		static std::string msg_wave_complete = "Wave complete";
		static std::string msg_tmp;
		static tick_t started = 0;
		wave_message.x = plr::cx() - 550;
		wave_message.y = plr::cy() - 250;
		tick_t ticks_left = 0;
		switch(game_state) {
			case GS_CHOOSE_DIFFICULTY:
				display_difficulty_prompt();
				break;
			case GS_INCOMING_WAVE:
				if(start_game_tick < tick::get()) {
					game->start_wave();
					game_state = GS_WAVE_ACTIVE;
					break;
				}

				ticks_left = start_game_tick - tick::get();
				if(ticks_left < 1000) {
					font::green_text(&wave_message,"go!",50,100);
					return;
				}
				msg_tmp = std::to_string(ticks_left)[0];
				font::green_text(&wave_message,msg_tmp,50,900);
				break;
			case GS_WAVE_ACTIVE:
				if(npc::alive_count() == 0) {
					npc::cleanup_corpses();
					game->next_wave();
					if(game->over()) {
						game_state = GS_WAVES_DONE;
						timer_tick = tick::get() + 5000;
						return;
					}
					game_state = GS_INCOMING_WAVE;
					start_game_tick = tick::get() + 5000;
				}
				break;
			case GS_WAVE_COMPLETE:
				font::green_text(&wave_message,msg_wave_complete,50,900);
				break;
			case GS_WAVE_REWARDS:
				std::cout << "[GS_WAVE_REWARDS](STUB)\n";
				break;
			case GS_WAVES_DONE:
				msg_tmp = "Wave complete[";
				ticks_left = timer_tick - tick::get();
				if(ticks_left < 1000) {
					msg_tmp += "0]";
				} else {
					msg_tmp += std::to_string(ticks_left)[0];
					msg_tmp += "]";
				}
				font::green_text(&wave_message,msg_tmp,50,900);
				if(timer_tick <= tick::get()) {
					game_state = GS_CHOOSE_DIFFICULTY;
					return;
				}
				break;
			case GS_CHOOSE_NEXT_AREA:
				std::cout << "[GS_CHOOSE_NEXT_AREA](STUB)\n";
				break;
			default:
				std::cout << "unhandled: " << __FILE__ << ":" << __LINE__ << "\n";
				break;
		}
	}
}; // end namespace gameplay
