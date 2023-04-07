#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "player.hpp"
#include "direction.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include <map>

#include "circle.hpp"
#include "npc-spetsnaz.hpp"
#include "mp5.hpp"
#include "cursor.hpp"
#include "bullet.hpp"
#include "draw.hpp"
#include "color.hpp"
#include "draw-state/player.hpp"
#include "draw-state/reticle.hpp"

static floatPoint ms_point ;
static floatPoint plr_point ;
static floatPoint top_right;
static floatPoint bot_right;
static constexpr int SCALE = 2;
static constexpr int W = 59 * SCALE;
static constexpr int H = 23 * SCALE;
static constexpr int GUN_ORIGIN_X_OFFSET = 20* SCALE;
static constexpr int GUN_ORIGIN_Y_OFFSET = 15* SCALE;

Player::Player(int32_t _x,int32_t _y,const char* _bmp_path,int _base_movement_amount) :
	self(_x,_y,_bmp_path) {
#ifdef SHOW_PLR_DIMENSIONS
	std::cout << "W: " << W << "\n";
	std::cout << "H: " << H << "\n";
#endif
	self.rect.w = W;
	self.rect.h = H;
	self.rect.x = (win_width() / 2) - (self.rect.w);
	self.rect.y = (win_height() / 2) - (self.rect.h);
	movement_amount = _base_movement_amount;

	firing_weapon = 0;
	hp = STARTING_HP;
	armor = STARTING_ARMOR;
	ready = true;
}
bool Player::weapon_should_fire() {
	return mp5.should_fire();
}
uint32_t Player::weapon_stat(WPN index) {
	return (*(mp5.stats))[index];
}
weapon_stats_t* Player::weapon_stats() {
	return mp5.stats;
}
int Player::gun_damage() {
	return rand_between(mp5.dmg_lo(),mp5.dmg_hi());
}

SDL_Texture* Player::initial_texture() {
	return this->self.bmp[0].texture;
}
void Player::calc() {
	cx =  self.rect.x + W / SCALE;
	cy =  self.rect.y + H / SCALE;
}
void Player::calc_outline() {
	calc();
	outline[0].x = cx;
	outline[0].y = cy;

	outline[1].x = cx - 20;
	outline[1].y = cy + 20;

	outline[2].x = cx - 20;
	outline[2].y = cy + 50;

	outline[3].x = cx + 20;
	outline[3].y = cy + 50;

	outline[4].x = cx + 20;
	outline[4].y = cy + 20;

	outline[5].x = cx;
	outline[5].y = cy;

	auto tmp_angle = angle;
	if(tmp_angle >= 247.5 && tmp_angle <= 292.5) {
		tmp_angle = 0;
	}
	tmp_angle = tmp_angle * PI / 180;
	float x,y;
	for(std::size_t i=0; i < OUTLINE_POINTS; i++) {
		x = outline[i].x;
		y = outline[i].y;
		outline[i].x = cx + ((x - cx) * cos(tmp_angle) - (y - cy) * sin(tmp_angle));
		outline[i].y = cy + ((x - cx) * sin(tmp_angle) + (y - cy) * cos(tmp_angle));
	}
}


namespace plr {
	static Player* p;
	Actor* self() {
		return &p->self;
	}
	int& movement_amount() {
		return p->movement_amount;
	}
	int gun_damage() {
		return p->gun_damage();
	}
	void start_gun() {
		p->firing_weapon = true;
	}
	void stop_gun() {
		p->firing_weapon = false;
	}
	uint32_t ms_registration() {
		return (*(p->weapon_stats()))[WPN_MS_REGISTRATION];
	}
	bool should_fire() {
		return p->firing_weapon;
	}
	void fire_weapon() {
		if(p->weapon_should_fire()) {
			bullet::queue_bullets(p->weapon_stats());
		}
	}
	void draw_outline() {
		save_draw_color();
		set_draw_color("red");
		SDL_RenderDrawLinesF(ren,
		                     &p->outline[0],
		                     OUTLINE_POINTS
		                    );
		restore_draw_color();
	}
	void rotate_guy() {
		p->angle = coord::get_angle(p->cx,p->cy,cursor::mx(),cursor::my());
		if(draw_state::player::draw_guy()) {
			SDL_RenderCopyEx(
			    ren,  //renderer
			    p->self.bmp[0].texture,
			    nullptr,// src rect
			    &p->self.rect,
			    p->angle, // angle
			    nullptr,  // center
			    SDL_FLIP_NONE // flip
			);
		}

		p->calc_outline();
	}
	void set_guy(Player* g) {
		p = g;
	}
	int& cx() {
		return p->cx;
	}
	int& cy() {
		return p->cy;
	}
	int& get_cx() {
		return p->cx;
	}
	int& get_cy() {
		return p->cy;
	}
	void calc() {
		p->calc();
	}
	SDL_Rect* get_rect() {
		return &p->self.rect;
	}
	void take_damage(weapon_stats_t * stats) {
		p->hp -= rand_between(stats);
	}

	void redraw_guy() {
		if(draw_state::player::draw_guy()) {
			SDL_RenderCopyEx(
			    ren,  //renderer
			    p->self.bmp[0].texture,
			    nullptr,// src rect
			    &p->self.rect,
			    p->angle, // angle
			    nullptr,//&center,  // center
			    SDL_FLIP_NONE// flip
			);
		}

#ifdef DRAW_OUTLINE
		draw_outline();
#endif
	}
	void draw_collision_outline(SDL_Rect* _proposed_outline) {
#ifdef DRAW_COLLISIONS
		draw::draw_green();
		SDL_RenderDrawRect(ren,_proposed_outline);
		draw::restore_color();
#endif

	}

	void restore_collision_outline(SDL_Rect* _result) {
#ifdef DRAW_COLLISIONS
		draw::draw_red();
		SDL_RenderDrawRect(ren,_result);
		draw::restore_color();
#endif
	}
	void draw_player_rects() {
		save_draw_color();
		set_draw_color("green");
		SDL_RenderDrawRect(ren,&p->self.rect);
		set_draw_color("red");
		restore_draw_color();
	}
	void draw_reticle() {
		if(draw_state::reticle::draw_reticle()) {
			save_draw_color();
			set_draw_color("red");
			shapes::DrawCircle(p->cx,p->cy,51);
			restore_draw_color();
			uint8_t r,g,b,a;
			SDL_GetRenderDrawColor(ren,&r,&g,&b,&a);
			SDL_SetRenderDrawColor(ren,255,0,0,0);
			SDL_RenderDrawLine(ren,
			                   p->cx,
			                   p->cy,
			                   cursor::mx(),
			                   cursor::my());
			auto color = colors::green();
			SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
			SDL_RenderDrawLine(ren,
			                   p->cx,
			                   p->cy,
			                   top_right.x,
			                   top_right.y
			                  );
			/**
			 * Draw a line between (cx,0)
			 */
			/* Draw line up to north */
			draw::line(p->cx,p->cy,p->cx,0);
			// draw line right to east
			draw::line(p->cx,p->cy,win_width(),p->cy);
			// draw line down to south
			draw::line(p->cx,p->cy,p->cx,win_height());
			// draw line left to west
			draw::line(p->cx,p->cy,0,p->cy);

			//draw_player_rects();
			SDL_SetRenderDrawColor(ren,r,g,b,a);
		}
	}
	int get_scale() {
		return ::SCALE ;
	}
	int get_width() {
		return ::W;
	}
	int get_height() {
		return ::H;
	}
	SDL_Rect* get_effective_rect() {
		static constexpr auto HALF_RADIAN = dir::HALF_RADIAN;
		static SDL_Rect r;
		r = p->self.rect;
		if(p->angle <= 450 - HALF_RADIAN && p->angle >= 405 - HALF_RADIAN) {
			/** Facing SOUTH EAST */
			r.y -= plr::get_height() / 1.3;
			r.w -= plr::get_width() / 2;
			r.h += plr::get_height() / 4;
		} else if(p->angle >= 360 - (HALF_RADIAN) && p->angle <= 360 + (HALF_RADIAN)) {
			/** FACING EAST */
			//r = p->self.rect;
			r.w -= plr::get_width() / 2;
		} else if(p->angle >= 315 - HALF_RADIAN && p->angle <= 360 - HALF_RADIAN) {
			/** FACING NORTH EAST */
			r.w -= plr::get_width() / 2;
			r.h += plr::get_height() / 2;
		} else if(p->angle >= 270 - HALF_RADIAN && p->angle <= 315 - HALF_RADIAN) {
			/** FACING NORTH */
			r.w -= plr::get_width() / 2;
			r.h += plr::get_height() / 2;
			r.x += plr::get_width() / 4;
			r.y += plr::get_height() / 4;
		} else if(p->angle >= 225 - HALF_RADIAN && p->angle <= 270 - HALF_RADIAN) {
			/** FACING NORTH WEST */
			r.x += plr::get_width() / 2;
			r.y += plr::get_height() / 2;
			r.w -= plr::get_width() / 2;
		} else if(p->angle >= 180 - HALF_RADIAN && p->angle <= 225 - HALF_RADIAN) {
			/** FACING WEST */
			//r = p->self.rect;
			r.w -= plr::get_width() / 2;
			r.x += plr::get_width() / 2;
		} else if(p->angle >= 135 - HALF_RADIAN && p->angle <= 180 - HALF_RADIAN) {
			/** FACING SOUTH WEST */
			r.x += plr::get_width() / 2;
			r.y -= plr::get_height() / 2;
			r.w -= plr::get_width() / 2;
		} else {
			/** FACING SOUTH */
			r.x += plr::get_width() / 4;
			r.y -= plr::get_height() / 1.3;
			r.w -= plr::get_width() / 2;
			r.h += plr::get_height() / 2;
		}
		return &r;
	}
	SDL_Rect* get_effective_move_rect() {
		/**
		 * FIXME: this is hacky, but _good enough_ to work
		 */
		static constexpr int HALF_RADIAN = 45 / 2;
		static SDL_Rect r;
		r = p->self.rect;
		r.y -= plr::get_height() / 2;
		r.w -= plr::get_width() / 2;
		r.w *= 2;
		r.h += plr::get_height() / 4;
		r.h *= 2;
		r.x += 10;
		r.w -= 20;
		r.h -= 10;
		return &r;
	}
};
