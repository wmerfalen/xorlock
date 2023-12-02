#ifndef __DAMAGE_EXPLOSIONS_HEADER__
#define __DAMAGE_EXPLOSIONS_HEADER__
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include "../actor.hpp"
#include "../world.hpp"
#include "../coordinates.hpp"
#include "../debug.hpp"
#include "../draw.hpp"
#include "../weapons.hpp"
#include "../constants.hpp"

namespace damage::explosions {
  void move_map(int direction,int amount);
  static constexpr std::size_t top_level_dir_first = 0;
	static constexpr const char* top_level_dir_pattern = "../assets/explosion-::XX::/%d.bmp";
  static constexpr const char* top_level_dp_replace = "::XX::";

  void detonate_at(SDL_Point* p,int damage,int type);
	struct explosion {
		SDL_Texture* initial_texture();
		void calculate_damage();
		void tick();
		Asset* next_state();
    void move_to(const int32_t& x,const int32_t& y);
    void trigger_explosion();

    Actor self;
    int angle;
		int explosive_damage;
    int radius;
		int x;
		int y;
    int phase;
    uint64_t start_tick;
		std::vector<Asset*> states;
    bool done;

		explosion(uint8_t directory_id);
		/** Copy constructor */
		explosion(const explosion& other) = delete;

	};

	void init();
	void tick();
  void space_bar_pressed();
};
#endif
