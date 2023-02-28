#ifndef __NPC_SPETSNAZ_HEADER__
#define __NPC_SPETSNAZ_HEADER__
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
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

	static constexpr int SPETSNAZ_MAX_HP = 100;
	static constexpr int SPETSNAZ_LOW_HP = 75;
	static constexpr int SPETSNAZ_RANDOM_LO = 10;
	static constexpr int SPETSNAZ_RANDOM_HI = 25;

	std::vector<Actor*> dead_list;
	struct Spetsnaz {
		wpn::MP5 mp5;
		struct Hurt {
			Actor self;
		};
		struct Dead {
			Actor self;
		};

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
		const bool is_dead() const {
			return hp <= 0;
		}
		uint32_t weapon_stat(WPN index) {
			return (*(mp5.stats))[index];
		}
		weapon_stats_t* weapon_stats() {
			return mp5.stats;
		}
		int gun_damage() {
			return rand_between(mp5.dmg_lo(),mp5.dmg_hi());
		}


		Spetsnaz(const int32_t& _x,
		         const int32_t& _y,
		         const int& _ma,
		         const npc_id_t& _id) {
			self.rect.x = _x;
			self.rect.y = _y;
			self.rect.w = SPETS_WIDTH;
			self.rect.h = SPETS_HEIGHT;
			movement_amount = _ma;
			self.load_bmp_asset(BMP);

			hurt_actor.self.load_bmp_assets(HURT_BMP,HURT_BMP_COUNT);
			dead_actor.self.load_bmp_assets(DEAD_BMP,DEAD_BMP_COUNT);
			hp = rand_between(SPETSNAZ_LOW_HP,SPETSNAZ_MAX_HP);
			max_hp = hp + rand_between(SPETSNAZ_RANDOM_LO,SPETSNAZ_RANDOM_HI);
			ready = true;

			state_index = 0;
			for(int i=0; i < hurt_actor.self.bmp.size(); ++i) {
				states.emplace_back(&hurt_actor.self.bmp[i]);
			}
			id = _id;
			calc();
		}
		Spetsnaz() : ready(false) {}
		/** Copy constructor */
		Spetsnaz(const Spetsnaz& other) = delete;

		SDL_Texture* initial_texture() {
			return self.bmp[0].texture;
		}
		void calc() {
			plr::calc();
			cx = self.rect.x + self.rect.w / 2;
			cy = self.rect.y + self.rect.h / 2;
			angle = coord::get_angle(cx,cy,plr::get_cx(),plr::get_cy());
		}
		void tick() {
			if(is_dead()) {
				return;
			}
			calc();
			perform_ai();
		}
		Asset* next_state() {
			if(state_index == states.size()) {
				return &dead_actor.self.bmp[0];
			}
			return states[state_index++];
		}

		void take_damage(int damage) {
			hp -= damage;
			if(hp <= 0) {
				self.bmp[0] = dead_actor.self.bmp[rand_between(0,dead_actor.self.bmp.size()-1)];
				dead_list.emplace_back(&self);
				return;
			}
			self.bmp[0] = *next_state();
		}
		void perform_ai();
		void move_left() {
			self.rect.x -= movement_amount;
		}
		void move_right() {
			self.rect.x += movement_amount;
		}
		void fire_at_player();
	};
	static std::forward_list<Spetsnaz> spetsnaz_list;
	static std::vector<Spetsnaz*> alive_list;

	template <typename TAttacker,typename TVictim>
	struct Travelers {
		int pixels_per_tick;
		int x;
		int y;
		int current_x;
		int current_y;
		bool done;
		Line line;
		std::vector<Point> points;
		std::size_t point_ctr;
		Travelers() : done(true) {}
		TAttacker* atkr;
		TVictim* vict;

		void attack_target(TVictim* _vict) {
			vict = _vict;
			atkr->calc();
			vict->calc();
			pixels_per_tick = atkr->pixels_per_tick();
			static constexpr double PI = 3.14159265358979323846;  // FIXME: Goes in math lib
			current_x = atkr->cx;
			current_y = atkr->cy;
			int angle = coord::get_angle(*atkr,vict->cx,vict->cy);
			atkr->angle = angle;
			x = 99500 * cos(PI * 2  * angle / 360);
			y = 99500 * sin(PI * 2 * angle / 360);
#ifdef DEBUG_GUN_LINE
			draw::line(current_x,current_y,x,y);
#endif
			line.p1 = Point {current_x,current_y};
			line.p2 = Point{x,y};
			line.angle = angle;
			points = line.getPoints(rand_between(1100,2180));
			point_ctr = 0;
			done = false;
		}
		Travelers(const Travelers& o) = delete;
		~Travelers() = default;

		const Point& next_point() {
			if(point_ctr >= points.size()) {
				point_ctr = 0;
				done = true;
			}
			return points[point_ctr++];
		}
		std::string report() {
			std::string s;
#ifdef DEBUG
			s += "x: ";
			s += std::to_string(x);
			s += "y: ";
			s += std::to_string(y);
			s += "current_x: ";
			s += std::to_string(current_x);
			s += "current_y: ";
			s += std::to_string(current_y);
#endif
			return s;
		}
	};

	int rand_spetsnaz_x() {
		return rand_between(-5000,win_width() * rand_between(1,10));
	}

	int rand_spetsnaz_y() {
		return rand_between(-5000,win_height() * rand_between(1,10));
	}

	void init_spetsnaz() {

	}
	void spawn_spetsnaz(const int& in_start_x, const int& in_start_y) {
		spetsnaz_list.emplace_front(in_start_x,in_start_y,SPETS_MOVEMENT,npc_id::next());
		world->npcs.push_front(&spetsnaz_list.front().self);
	}
	void Spetsnaz::fire_at_player() {
		calc();
#ifdef DRAW_SPETSNAZ_PREFIRE_LINE
		draw::line(cx,cy,plr::get_cx(),plr::get_cy());
#endif
		bullet::queue_npc_bullets(id,weapon_stats(),cx,cy,plr::get_cx(),plr::get_cy());
	}
	void Spetsnaz::perform_ai() {
		if(plr::get_cx() < cx) {
			move_left();
		}
		if(plr::get_cx() > cx) {
			move_right();
		}
		if(plr::get_cx() == cx) {
			fire_at_player();
		}
	}
	void spetsnaz_tick() {
		for(auto& s : spetsnaz_list) {
			s.tick();
			SDL_RenderCopyEx(
			    ren,  //renderer
			    s.self.bmp[0].texture,
			    nullptr,// src rect
			    &s.self.rect,
			    s.angle, // angle
			    nullptr,  // center
			    SDL_FLIP_NONE // flip
			);
		}
	}
	void spetsnaz_movement(uint8_t dir,int adjustment) {
		for(auto& s : spetsnaz_list) {
			if(s.is_dead()) {
				continue;
			}
			if(dir == EAST || dir == WEST) {
				s.self.rect.x += adjustment;
			} else {
				s.self.rect.y += adjustment;
			}
		}
	}
	void take_damage(Actor* a,int dmg) {
		for(auto& s : spetsnaz_list) {
			if(&s.self == a) {
				s.take_damage(dmg);
			}
		}
	}
	bool is_dead(Actor* a) {
		return std::find(dead_list.cbegin(), dead_list.cend(), a) != dead_list.cend();
	}
};
#endif
