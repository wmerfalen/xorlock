#include <SDL2/SDL.h>
#include <iostream>
#include "npc-spetsnaz.hpp"
#include "player.hpp"
#include "direction.hpp"
#include "npc/paths.hpp"

namespace npc {
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
	void Spetsnaz::calculate_aim() {
		target_x = plr::get_cx();
		target_y = plr::get_cy();
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
		plr::calc();
		calc();
#ifdef DRAW_SPETSNAZ_PREFIRE_LINE
		draw::line(cx,cy,target_x,target_y);
#endif

		bullet::queue_npc_bullets(id,weapon_stats(),cx,cy,target_x,target_y);
	}
	void Spetsnaz::aim_at_player() {
		draw::line(cx,cy,target_x,target_y);
	}
	void Spetsnaz::get_hit() {
		m_stunned_until = STUNNED_TICKS + rand_between(200,500) + tick::get();
	}
	void Spetsnaz::take_damage(int damage) {
		get_hit();
		hp -= damage;
		if(hp <= 0) {
			self.bmp[0] = dead_actor.self.bmp[rand_between(0,dead_actor.self.bmp.size()-1)];
			dead_list.emplace_back(&self);
			return;
		}
		self.bmp[0] = *next_state();
	}
	float Spetsnaz::aiming_range_multiplier() {
		/**
		 * TODO: add randomness
		 */
		return AIMING_RANGE_MULTIPLIER;
	}
	bool Spetsnaz::within_aiming_range() {
		calc(); // FIXME: do this once per tick
		static const auto& _px = plr::get_cx();
		return _px <= cx + (center_x_offset() * aiming_range_multiplier()) && _px >= cx - (center_x_offset() * aiming_range_multiplier());
	}
	void Spetsnaz::move_to(SDL_Point* in_point) {
		self.rect.x = in_point->x;
		self.rect.y = in_point->y;
	}
	void Spetsnaz::show_confused() {
		static SDL_Point p;
		p.x = self.rect.x;
		p.y = self.rect.y;
		p.x += 90;
		p.y -= 100;
		draw::bubble_text(&p,"huh?!?!");
	}
	void Spetsnaz::update_check() {
		static uint16_t call_count = 0;
		if(++call_count >=80) {
			call_count = 0;
		} else {
			return;
		}
		path_finder.update(&self,plr::self());
		auto valid_points= path_finder.calculate_path();
		++pf_index;
		if(pf_index >= valid_points) {
			pf_index = 0;
		}
		move_to(&path_finder.points[pf_index]);
	}
	void Spetsnaz::perform_ai() {
		if(m_stunned_until > tick::get()) {
			return;
		}
		update_check();
		//if(within_aiming_range()) {
		//	calculate_aim();
		//	aim_at_player();
		//}
		//if(within_range() && can_fire_again()) {
		//	fire_at_player();
		//}
	}
	void spetsnaz_tick() {
		for(auto& s : spetsnaz_list) {
			s.tick();
			s.path_finder.animate();
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
			adjustment *= SPETSNAZ_ADJUSTMENT_MULTIPLIER;
			switch(dir) {
				case NORTH_WEST:
					s.self.rect.x += abs(adjustment);
					s.self.rect.y += abs(adjustment);
					break;
				case NORTH_EAST:
					s.self.rect.x -= abs(adjustment);
					s.self.rect.y += abs(adjustment);
					break;
				case SOUTH_EAST:
					s.self.rect.x -= abs(adjustment);
					s.self.rect.y -= abs(adjustment);
					break;
				case SOUTH_WEST:
					s.self.rect.x += abs(adjustment);
					s.self.rect.y -= abs(adjustment);
					break;
				case WEST:
					s.self.rect.x += abs(adjustment);
					break;
				case EAST:
					s.self.rect.x -= abs(adjustment);
					break;
				case SOUTH:
					s.self.rect.y -= abs(adjustment);
					break;
				case NORTH:
					s.self.rect.y += abs(adjustment);
					break;
			}
			s.calc();
		}
	}
	SDL_Texture* Spetsnaz::initial_texture() {
		return self.bmp[0].texture;
	}
	void Spetsnaz::calc() {
		plr::calc();
		cx = self.rect.x + self.rect.w / 2;
		cy = self.rect.y + self.rect.h / 2;
		angle = coord::get_angle(cx,cy,plr::get_cx(),plr::get_cy());
	}
	void Spetsnaz::tick() {
		if(is_dead()) {
			return;
		}
		calc();
		perform_ai();
	}
	Asset* Spetsnaz::next_state() {
		if(hp <= 0) {
			return &dead_actor.self.bmp[0];
		}
		return states[0];
	}
	void Spetsnaz::move_south() {
		self.rect.y += movement_amount;
	}
	void Spetsnaz::move_north() {
		self.rect.y -= movement_amount;
	}

	void Spetsnaz::move_left() {
		self.rect.x -= movement_amount;
	}
	void Spetsnaz::move_right() {
		self.rect.x += movement_amount;
	}
	int Spetsnaz::center_x_offset() {
		return CENTER_X_OFFSET;
	}
	const bool Spetsnaz::is_dead() const {
		return hp <= 0;
	}
	uint32_t Spetsnaz::weapon_stat(WPN index) {
		return (*(mp5.stats))[index];
	}
	weapon_stats_t* Spetsnaz::weapon_stats() {
		return mp5.stats;
	}
	int Spetsnaz::gun_damage() {
		return rand_between(mp5.dmg_lo(),mp5.dmg_hi());
	}

	Spetsnaz::Spetsnaz() : path_finder(npc::paths::PathFinder{SPETS_MOVEMENT,&self,plr::self()}) {
		ready = false;
		last_aim_tick = tick::get();
	}
	Spetsnaz::Spetsnaz(const int32_t& _x,
	                   const int32_t& _y,
	                   const int& _ma,
	                   const npc_id_t& _id) : path_finder(npc::paths::PathFinder{SPETS_MOVEMENT,&self,plr::self()}) {
		self.rect.x = _x;
		self.rect.y = _y;
		self.rect.w = SPETS_WIDTH;
		self.rect.h = SPETS_HEIGHT;
		movement_amount = _ma;
		self.load_bmp_asset(BMP);

		hurt_actor.self.load_bmp_assets(HURT_BMP,HURT_BMP_COUNT);
		dead_actor.self.load_bmp_assets(DEAD_BMP,DEAD_BMP_COUNT);
		hp = SPETSNAZ_LOW_HP;
		max_hp = SPETSNAZ_MAX_HP;
		ready = true;

		state_index = 0;
		for(int i=0; i < hurt_actor.self.bmp.size(); ++i) {
			states.emplace_back(&hurt_actor.self.bmp[i]);
		}
		id = _id;
		calc();
		m_last_fire_tick = 0;
		m_stunned_until = 0;
		last_aim_tick = tick::get();
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
