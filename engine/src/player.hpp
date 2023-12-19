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

#include "circle.hpp"
#include "npc-spetsnaz.hpp"
#include "weapons/smg/mp5.hpp"
#include "weapons/pistol.hpp"
#include "weapons/grenade/frag.hpp"
#include "weapons/grenade.hpp"
#include "cursor.hpp"
#include "bullet.hpp"
#include "draw.hpp"
#include "draw-state/player.hpp"
#include "draw-state/reticle.hpp"
#include "weapons.hpp"
#include "reload.hpp"
#include <memory>
#include "backpack.hpp"

static constexpr double PI = 3.14159265358979323846;
static constexpr std::size_t BULLET_POOL_SIZE = 24;

bool between(int target, int min,int max);
void save_draw_color();
void restore_draw_color();
void set_draw_color(const char* s);
static constexpr int BASE_MOVEMENT_AMOUNT = CELL_WIDTH / 16;
static constexpr int RUN_MOVEMENT_AMOUNT = CELL_WIDTH / 12;

//static Actor bcom;
static constexpr std::size_t OUTLINE_POINTS = 6;
struct Player {
	static constexpr int GUN_DAMAGE_RANDOM_LO = 45;
	static constexpr int GUN_DAMAGE_RANDOM_HI = 75;
	static constexpr int16_t STARTING_HP = 100;
	static constexpr int16_t STARTING_ARMOR = 10;
	Player(int32_t _x,int32_t _y,const char* _bmp_path,int _base_movement_amount);

	std::unique_ptr<weapons::smg::MP5> mp5;
  std::unique_ptr<weapons::Pistol> pistol;
  std::unique_ptr<weapons::grenade::Frag> frag;
  std::unique_ptr<weapons::Grenade> grenade_manager;
	std::string equipped_weapon_name;
	std::function<const bool()> lambda_should_fire;
	std::function<const uint32_t& (const uint8_t&)> lambda_stat_index;
	weapon_stats_t* wpn_stats;
  explosive_stats_t* exp_stats;
	std::function<int()> lambda_dmg_lo;
	std::function<int()> lambda_dmg_hi;
	Actor self;
	std::array<SDL_FPoint,OUTLINE_POINTS> outline;
	int movement_amount;
	int cx;
	int cy;
	int angle;
	bool ready;
	bool firing_weapon;
	bool running;
	int16_t hp;
	int16_t armor;
	uint32_t clip_size;
	uint16_t* ammo;
	uint16_t* total_ammo;
  uint32_t equipped_weapon;
  int16_t weapon_index;
  bool changing_weapon;
  int current_equipped_weapon;
  int target_equipped_weapon;
  uint64_t holding_grenade_at;
  int equip_weapon(int index);
  int equip_weapon(int index,weapon_stats_t* wpn,explosive_stats_t* exp);
  int start_equip_weapon(int index);
  std::vector<wpn::weapon_t> inventory;
  weapon_stats_t* primary;
  weapon_stats_t* secondary;
  explosive_stats_t* explosive_0;
  explosive_stats_t* explosive_1;

  uint64_t has_target_at;
  
	std::unique_ptr<reload::ReloadManager> reloader;
  std::unique_ptr<backpack::Backpack> backpack;
  // TODO: add method which will place item into backpack
  // TODO: once added item to backpack, save to disk
	void weapon_click();
	//void equip_weapon(const wpn::weapon_t& _weapon);
	void consume_ammo();
	// TODO: primary/secondary
	void unequip_weapon(const wpn::position_t& _pos);
	bool weapon_should_fire();
	uint32_t weapon_stat(WPN index);
	weapon_stats_t* weapon_stats();
  std::pair<int,int> gun_damage();

	Player() = delete;
  ~Player();
	/** Copy constructor */
	Player(const Player& other) = delete;


	SDL_Texture* initial_texture();
	void calc();
	void calc_outline();

  void start_equip_secondary();
  void start_equip_primary();
  void start_equip_frag();
  void unequip_primary();
  void unequip_secondary();
  void tick();

  void cycle_previous_weapon();
  void cycle_next_weapon();
};

namespace plr {
	int& movement_amount();
	void run(bool t);
  std::pair<int,int> gun_damage();
	void start_gun();
	void stop_gun();
	uint32_t ms_registration();
	uint16_t ammo();
	uint16_t total_ammo();
	bool should_fire();
	void draw_outline();
	void rotate_guy();
	void set_guy(Player* g);
	int& cx();
	int& cy();
	int& get_cx();
	int& get_cy();
	void calc();
	SDL_Rect* get_rect();
	void take_damage(weapon_stats_t * stats);
	Player* get();
	void update_reload_state(const reload::reload_phase_t& phase);

	void redraw_guy();
	void draw_player_rects();
	void draw_reticle();
	void draw_collision_outline(SDL_Rect* _proposed_outline);
	void restore_collision_outline(SDL_Rect* _result);
	int get_scale();
	int get_width();
	int get_height();
	Actor* self();
	SDL_Rect* get_effective_rect();
	SDL_Rect* get_effective_move_rect();

  void tick();
};
#endif
