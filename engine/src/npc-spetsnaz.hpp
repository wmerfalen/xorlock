#ifndef __NPC_SPETSNAZ_HEADER__
#define __NPC_SPETSNAZ_HEADER__
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include "actor.hpp"
#include "world.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include "debug.hpp"
#include "draw.hpp"
#include "mp5.hpp"
#include "extern.hpp"
#include "behaviour-tree.hpp"
#include "npc-id.hpp"

namespace npc {
	static constexpr std::size_t SPETSNAZ_MAX = 16;
	static constexpr std::size_t SPETS_WIDTH = 80;
	static constexpr std::size_t SPETS_HEIGHT = 53;
	static constexpr std::size_t SPETS_MOVEMENT = 2;
	static constexpr std::size_t BULLET_POOL_SIZE = 1024;
	static constexpr const char* BMP = "../assets/spet-0.bmp";
	static constexpr const char* HURT_BMP = "../assets/spet-hurt-%d.bmp";
	static constexpr std::size_t HURT_BMP_COUNT = 3;
	static constexpr const char* DEAD_BMP = "../assets/spet-dead-%d.bmp";
	static constexpr std::size_t DEAD_BMP_COUNT = 1;
	static constexpr int CENTER_X_OFFSET = 110;
	static constexpr uint16_t COOLDOWN_BETWEEN_SHOTS = 810;
	static constexpr float AIMING_RANGE_MULTIPLIER = 1.604;
	static constexpr uint16_t STUNNED_TICKS = 300;

	static constexpr int SPETSNAZ_MAX_HP = 100;
	static constexpr int SPETSNAZ_LOW_HP = 75;
	static constexpr int SPETSNAZ_RANDOM_LO = 10;
	static constexpr int SPETSNAZ_RANDOM_HI = 25;

	static std::vector<Actor*> dead_list;
	struct Spetsnaz {
		wpn::MP5 mp5;
		struct Hurt {
			Actor self;
		};
		bool within_range();
		bool within_aiming_range();
		float aiming_range_multiplier();
		struct Dead {
			Actor self;
		};

		uint64_t m_last_fire_tick;
		uint16_t cooldown_between_shots();
		bool can_fire_again();
		void aim_at_player();
		Actor self;
		int movement_amount;
		int cx;
		int cy;
		SDL_Rect& dest = self.rect;
		Hurt hurt_actor;
		Dead dead_actor;
		int hp;
		int max_hp;
		int angle;
		bool ready;
		std::vector<Asset*> states;
		std::size_t state_index;
		npc_id_t id;
		uint64_t m_stunned_until;
		const bool is_dead() const;
		uint32_t weapon_stat(WPN index);
		weapon_stats_t* weapon_stats();
		int gun_damage();


		Spetsnaz(const int32_t& _x,
		         const int32_t& _y,
		         const int& _ma,
		         const npc_id_t& _id);
		Spetsnaz();
		/** Copy constructor */
		Spetsnaz(const Spetsnaz& other) = delete;

		SDL_Texture* initial_texture();
		void calc();
		void tick();
		Asset* next_state();

		void get_hit();
		void take_damage(int damage);
		void perform_ai();
		void move_left();
		void move_right();
		void fire_at_player();
		int center_x_offset();
	};
	static std::forward_list<Spetsnaz> spetsnaz_list;

	const int center_x_offset();

	int rand_spetsnaz_x();
	int rand_spetsnaz_y();

	void spawn_spetsnaz(const int& in_start_x, const int& in_start_y);
	void init_spetsnaz();
	void spetsnaz_tick();
	void spetsnaz_movement(uint8_t dir,int adjustment);
	void take_damage(Actor* a,int dmg);
	bool is_dead(Actor* a);
};
#endif
