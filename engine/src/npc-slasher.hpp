#ifndef __NPC_BOMBER_HEADER__
#define __NPC_BOMBER_HEADER__
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

#include "behaviour-tree.hpp"
#include "npc-id.hpp"
#include "npc/paths.hpp"
#include "weapons.hpp"
#include "weapons/primary.hpp"
#include "constants.hpp"

namespace npc {
	struct Bomber {

    /**
     * In reality, the Bomber does no slashing whatsoever. It currently just explodes
     * when it's near the player. Please note that melee NPCs are a game dynamic that
     * is very much desired... it's just I haven't found a good way of dealing with
     * that.
     */
    static constexpr constants::npc_type_t TYPE_ID = constants::npc_type_t::NPC_BOMBER;
		weapons::Primary machete;
		struct Hurt {
			Actor self;
      ~Hurt(){
        return;
      }
		};
		struct Dead {
			Actor self;
      ~Dead(){
        return;
      }
		};

    bool blocked;
		uint64_t m_last_slash_tick;
		uint16_t cooldown_between_shots();
		bool can_slash_again();
    bool within_aiming_range();
    void rush_at_player();
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
		const bool is_dead() const;
		uint32_t weapon_stat(WPN index);
		weapon_stats_t* weapon_stats();
		int target_x;
		int target_y;
    void die();
    bool dead();
    void corpse_hit();
		std::size_t pf_index;
    uint64_t last_vocal;
    uint64_t perform_ai_tick;


    ~Bomber();
		Bomber(const int32_t& _x,
		         const int32_t& _y,
		         const int& _ma,
		         const npc_id_t& _id);
		Bomber();
		/** Copy constructor */
		Bomber(const Bomber& other) = delete;

		SDL_Texture* initial_texture();
		void calc();
		void tick();
		Asset* next_state();
		SDL_Point next_path;
    SDL_Point* next_path_candidate;
    uint64_t wander_tick;
    uint64_t wander_started_tick;
    uint16_t wander_direction;
		std::array<wall::Wall*,npc::paths::ChosenPath::PATH_SIZE>* path;
    bool wandering_mode;
    void start_wandering();
		void walk_to_next_path();

		void take_damage(int damage);
		void take_explosive_damage(int damage,SDL_Rect* source_explosion,int blast_radius, int on_death,SDL_Rect* src_rect);
		void perform_ai();
		bool move_east();
		bool move_west();
		bool move_south();
		bool move_north();
    bool move_north_east();
    bool move_north_west();
    bool move_south_east();
    bool move_south_west();
    void report();
		void move_to(const int32_t& x,const int32_t& y);
		void slash_at_player();
		int center_x_offset();
		void update_check();
    void next_waypoint();
		std::unique_ptr<npc::paths::PathFinder> path_finder;
		bool can_see_player();
    void cleanup();
    bool is_slashing() const;
    std::vector<SDL_Point> trajectory;
    uint16_t velocity;
    std::size_t trajectory_index;
    uint8_t rush_charge;
    uint64_t last_rush_tick;
	};
  namespace bomber {

    const int center_x_offset();

    void spawn_bomber(const std::size_t& count);
    void init();
    void tick();
    void bomber_movement(uint8_t dir,int adjustment);
    void take_damage(Actor* a,int dmg);
    void take_explosive_damage(Actor* a,int damage,SDL_Rect* source_explosion,int blast_radius, int on_death,SDL_Rect* src_rect);
    bool is_dead(Actor* a);
    const std::size_t& dead_count() ;
    const std::size_t& alive_count() ;
    void cleanup_corpses();
    void program_exit();
    void move_map(int dir,int amt);
    void debug_click();
  };
};
#endif
