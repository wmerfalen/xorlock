#ifndef __PLAYER_HEADER__
#define __PLAYER_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include <map>
#include "extern.hpp"
#include "circle.hpp"
#include "npc-spetsnaz.hpp"
#include "mp5.hpp"
#include "cursor.hpp"
#include "bullet.hpp"
#include "draw.hpp"
#include "draw-state/player.hpp"
#include "draw-state/reticle.hpp"

static constexpr double PI = 3.14159265358979323846;
static constexpr std::size_t BULLET_POOL_SIZE = 24;
struct Player;

bool between(int target, int min,int max);
void save_draw_color();
void restore_draw_color();
void set_draw_color(const char* s);

static Actor bcom;
static constexpr std::size_t OUTLINE_POINTS = 6;
struct Player {
	static constexpr int GUN_DAMAGE_RANDOM_LO = 45;
	static constexpr int GUN_DAMAGE_RANDOM_HI = 75;
	static constexpr int16_t STARTING_HP = 10000;
	static constexpr int16_t STARTING_ARMOR = 1000;
	Player(int32_t _x,int32_t _y,const char* _bmp_path);
	wpn::MP5 mp5;
	Actor self;
	std::array<SDL_FPoint,OUTLINE_POINTS> outline;
	int movement_amount;
	int cx;
	int cy;
	int angle;
	bool ready;
	bool firing_weapon;
	int16_t hp;
	int16_t armor;
	bool weapon_should_fire();
	uint32_t weapon_stat(WPN index);
	weapon_stats_t* weapon_stats();
	int gun_damage();

	Player();
	/** Copy constructor */
	Player(const Player& other) = delete;


	SDL_Texture* initial_texture();
	void calc();
	void calc_outline();

};


namespace plr {
	using namespace static_guy;
	int movement_amount();
	int gun_damage();
	void start_gun();
	void stop_gun();
	uint32_t ms_registration();
	bool should_fire();
	void fire_weapon();
	void draw_outline();
	void rotate_guy();
	void set_guy(Player* g);
	int cx();
	int cy();
	int get_cx();
	int get_cy();
	void calc();
	SDL_Rect* get_rect();
	void take_damage(weapon_stats_t * stats);

	void redraw_guy();
	void draw_player_rects();
	void draw_reticle();
};
#endif
