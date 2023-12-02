#ifndef __F35_HEADER__
#define __F35_HEADER__
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
#include "../weapons/smg/mp5.hpp"

#include "../behaviour-tree.hpp"
#include "../npc-id.hpp"
#include "../npc/paths.hpp"
#include "../weapons.hpp"
#include "../constants.hpp"

namespace air_support::f35 {
  void move_map(int direction,int amount);
	static constexpr uint32_t F35_CALL_COUNT = 180;
	static constexpr int F35_ADJUSTMENT_MULTIPLIER = 1.0;
	//static constexpr std::size_t F35_WIDTH = 550;
	//static constexpr std::size_t F35_HEIGHT = 330;
	static constexpr std::size_t F35_MOVEMENT = 2;
	static constexpr std::size_t BULLET_POOL_SIZE = 1024;
	static constexpr const char* BMP = "../assets/f35.bmp";
	static constexpr const char* HURT_BMP = "../assets/f35-hurt-%d.bmp";
	static constexpr std::size_t HURT_BMP_COUNT = 3;
	static constexpr const char* DEAD_BMP = "../assets/f35-dead-%d.bmp";
	static constexpr std::size_t DEAD_BMP_COUNT = 1;
	static constexpr int CENTER_X_OFFSET = 110;
	static constexpr uint16_t COOLDOWN_BETWEEN_SHOTS = 810;
	static constexpr float AIMING_RANGE_MULTIPLIER = 1.604;
	static constexpr uint16_t STUNNED_TICKS = 300;

	static constexpr int F35_MAX_HP = 100;
	static constexpr int F35_LOW_HP = 75;
	static constexpr int F35_RANDOM_LO = 10;
	static constexpr int F35_RANDOM_HI = 25;
	static constexpr int SEE_DISTANCE = 500;

	static std::vector<Actor*> dead_list;
  bool is_dispatched();
  std::size_t dispatch_now();
  void return_to_carrier();
	struct F35 {
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
    bool on_carrier;
		void calculate_aim();
		int gun_damage();
    bool m_dispatched;
    bool dispatched();
    void call_in_airstrike(std::size_t i);

		F35(const int32_t& _x,
		         const int32_t& _y);
		F35();
		/** Copy constructor */
		F35(const F35& other) = delete;

		SDL_Texture* initial_texture();
		void calc();
		void tick();
		Asset* next_state();
    uint32_t cooldown_between_shots();
    void move_to(const int32_t& x,const int32_t& y);

		void perform_ai();
    bool headed_back_to_base();
    std::vector<SDL_Point> bombing_targets;
    void randomize_bombing_targets();
	};
	static std::forward_list<F35> f35_list;

	void spawn(const int& in_start_x, const int& in_start_y);
	void init();
	void tick();
  void space_bar_pressed();
};
#endif
