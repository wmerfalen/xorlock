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
#include "weapons/smg/mp5.hpp"

#include "behaviour-tree.hpp"
#include "npc-id.hpp"
#include "npc/paths.hpp"
#include "weapons.hpp"
#include "constants.hpp"

namespace npc {
	struct Spetsnaz {
		weapons::smg::MP5 mp5;
    static constexpr constants::npc_type_t TYPE_ID = constants::npc_type_t::NPC_SPETSNAZ;
		struct Hurt {
			Actor self;
      ~Hurt(){
        return;
      }
		};
		bool within_range();
		bool within_aiming_range();
		float aiming_range_multiplier();
		struct Dead {
			Actor self;
      ~Dead(){
        return;
      }
		};

		uint32_t call_count;
		uint64_t m_last_fire_tick;
		uint16_t cooldown_between_shots();
		bool can_fire_again();
		void aim_at_player();
		void show_confused();
		Actor self;
		int movement_amount;
		int cx;
		int cy;
		SDL_Rect& dest = self.rect;
		Hurt hurt_actor;
		Dead dead_actor;
    bool dismembered;
		int hp;
		int max_hp;
		int angle;
		bool ready;
		std::vector<Asset*> states;
		std::size_t state_index;
		npc_id_t id;
		uint64_t m_stunned_until;
		uint64_t last_aim_tick;
		const bool is_dead() const;
		uint32_t weapon_stat(WPN index);
		weapon_stats_t* weapon_stats();
		int target_x;
		int target_y;
		void calculate_aim();
		int gun_damage();
    void die();
    bool dead();
    void corpse_hit();
		std::size_t pf_index;


    ~Spetsnaz();
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
		SDL_Point next_path;
		void walk_to_next_path();

		void take_damage(int damage);
		void take_explosive_damage(int damage,SDL_Rect* source_explosion,int blast_radius, int on_death,SDL_Rect* src_rect);
		void perform_ai();
		void move_left();
		void move_right();
		void move_south();
		void move_north();
		void move_to(SDL_Point* in_point);
		void move_to(const int32_t& x,const int32_t& y);
		void fire_at_player();
		int center_x_offset();
		void update_check();
		std::unique_ptr<npc::paths::PathFinder> path_finder;
		bool can_see_player();
    void cleanup();
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
  void take_explosive_damage(Actor* a,int damage,SDL_Rect* source_explosion,int blast_radius, int on_death,SDL_Rect* src_rect);
	bool is_dead(Actor* a);
	const std::size_t& dead_count() ;
	const std::size_t& alive_count() ;
	void cleanup_corpses();
  void program_exit();
  void move_map(int dir,int amt);
};
#endif
