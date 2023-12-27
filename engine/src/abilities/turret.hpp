#ifndef __ABILITIES_TURRET_HEADER__
#define __ABILITIES_TURRET_HEADER__
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include "../actor.hpp"
#include "../world.hpp"
#include "../coordinates.hpp"
#include "../bullet-pool.hpp"
#include "../debug.hpp"
#include "../draw.hpp"

#include "../behaviour-tree.hpp"
#include "../npc-id.hpp"
#include "../npc/paths.hpp"
#include "../weapons.hpp"
#include "../constants.hpp"

namespace abilities::turret {
  void move_map(int direction,int amount);

	static constexpr int TURRET_MAX_HP = 100;
	static constexpr int TURRET_LOW_HP = 75;
	static constexpr int TURRET_RANDOM_LO = 10;
	static constexpr int TURRET_RANDOM_HI = 25;
	static constexpr int SEE_DISTANCE = 500;

	static std::vector<Actor*> dead_list;
	struct Turret {
		uint32_t call_count;
		uint64_t m_last_fire_tick;
		bool can_fire_again();
		Actor self;
		int movement_amount;
		int cx;
		int cy;
		SDL_Rect& dest = self.rect;
		int hp;
		int max_hp;
		int angle;
		bool ready;
		std::vector<Asset*> states;
		std::size_t state_index;
		int target_x;
		int target_y;
    SDL_Point deployment_location;
    uint64_t deployed_at;
		void calculate_aim();
		int gun_damage();
    void deploy_at(const SDL_Point& p);

		Turret(const int32_t& _x,
		         const int32_t& _y);
		Turret() = delete;
		/** Copy constructor */
		Turret(const Turret& other) = delete;

		SDL_Texture* initial_texture();
		void calc();
		void tick();
		Asset* next_state();
    uint32_t cooldown_between_shots();
    void move_to(const int32_t& x,const int32_t& y);

		void perform_ai();
    bool dispatched;
    bool sweep_forward;
    int16_t start_angle;
	};

	void spawn(const int& in_start_x, const int& in_start_y);
	void init();
	void tick();
  void space_bar_pressed();
  void program_exit();
};
#endif
