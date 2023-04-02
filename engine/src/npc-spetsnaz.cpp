#include <SDL2/SDL.h>
#include <iostream>
#include "npc-spetsnaz.hpp"
//#include "actor.hpp"
//#include "world.hpp"
//#include "triangle.hpp"
//#include "coordinates.hpp"
//#include "bullet-pool.hpp"
//#include "debug.hpp"
//#include "draw.hpp"
//#include "mp5.hpp"
//#include "extern.hpp"
//#include "behaviour-tree.hpp"
//#include "npc-id.hpp"

namespace npc {
	//std::vector<Actor*> dead_list;
	//static std::forward_list<Spetsnaz> spetsnaz_list;

	bool Spetsnaz::within_range() {
		calc();
		static const auto& px = plr::get_cx();
		return px <= cx + center_x_offset() && px >= cx - center_x_offset();
	}
	int rand_spetsnaz_x() {
		return rand_between(-5000,win_width() * rand_between(1,10));
	}

	int rand_spetsnaz_y() {
		return rand_between(-5000,win_height() * rand_between(1,10));
	}

	void spawn_spetsnaz(const int& in_start_x, const int& in_start_y) {
		spetsnaz_list.emplace_front(in_start_x,in_start_y,SPETS_MOVEMENT,npc_id::next());
		world->npcs.push_front(&spetsnaz_list.front().self);
	}
	void init_spetsnaz() {
		spawn_spetsnaz(0,0);
	}
	uint16_t Spetsnaz::cooldown_between_shots() {
		return COOLDOWN_BETWEEN_SHOTS;
	}
	bool Spetsnaz::can_fire_again() {
		return m_last_fire_tick + cooldown_between_shots() <= tick::get();
	}
	void Spetsnaz::fire_at_player() {
		m_last_fire_tick = tick::get();
		calc();
#ifdef DRAW_SPETSNAZ_PREFIRE_LINE
		draw::line(cx,cy,plr::get_cx(),plr::get_cy());
#endif
		bullet::queue_npc_bullets(id,weapon_stats(),cx,cy,plr::get_cx(),plr::get_cy());
	}
	void Spetsnaz::aim_at_player() {
		draw::line(cx,cy,plr::get_cx(),plr::get_cy());
	}
	void Spetsnaz::get_hit() {
		m_stunned_until = STUNNED_TICKS + rand_between(200,500) + tick::get();
	}
	float Spetsnaz::aiming_range_multiplier() {
		/**
		 * TODO: add randomness
		 */
		return AIMING_RANGE_MULTIPLIER;
	}
	bool Spetsnaz::within_aiming_range() {
		calc(); // FIXME: do this once per tick
		static const auto& px = plr::get_cx();
		return px <= cx + (center_x_offset() * aiming_range_multiplier()) && px >= cx - (center_x_offset() * aiming_range_multiplier());
	}
	void Spetsnaz::perform_ai() {
		if(m_stunned_until > tick::get()) {
			return;
		}
		if(plr::get_cx() < cx) {
			move_left();
		}
		if(plr::get_cx() > cx) {
			move_right();
		}
		if(within_aiming_range()) {
			aim_at_player();
		}
		if(within_range() && can_fire_again()) {
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
