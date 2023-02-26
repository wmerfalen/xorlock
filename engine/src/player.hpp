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

static constexpr std::size_t BULLET_POOL_SIZE = 24;
struct Player;
namespace static_guy {
	static Player* p;
};

bool between(int target, int min,int max) {
	return target > min && target < max;
}
void save_draw_color() {
	using namespace saved;
	SDL_GetRenderDrawColor(ren,&r,&g,&b,&a);
}
void restore_draw_color() {
	using namespace saved;
	SDL_SetRenderDrawColor(ren,r,g,b,a);
}
void set_draw_color(const char* s) {
	if(strcmp("red",s) == 0) {
		SDL_SetRenderDrawColor(ren,255,0,0,0);
	}
}

static Actor bcom;
#define NORMAL_SIZE
#ifdef NORMAL_SIZE
static constexpr int SCALE = 2;
static constexpr int W = 59 * SCALE;
static constexpr int H = 23 * SCALE;
static constexpr int GUN_ORIGIN_X_OFFSET = 20* SCALE;
static constexpr int GUN_ORIGIN_Y_OFFSET = 15* SCALE;
static constexpr int W2 = ceil(W / 2);
static constexpr int H2 = ceil(H / 2);
#else
static constexpr int W = 77;
static constexpr int H = 77;
static constexpr int W2 = ceil(W / 2);
static constexpr int H2 = ceil(H / 2);
#endif
struct Player {
	static constexpr int GUN_DAMAGE_RANDOM_LO = 45;
	static constexpr int GUN_DAMAGE_RANDOM_HI = 75;
	static constexpr int16_t STARTING_HP = 10000;
	static constexpr int16_t STARTING_ARMOR = 1000;
	Player(int32_t _x,int32_t _y,const char* _bmp_path) :
		self(_x,_y,_bmp_path),
		movement_amount(10),
		ready(true) {
		self.rect.w = W;
		self.rect.h = H;
		self.rect.x = (win_width() / 2) - (self.rect.w);
		self.rect.y = (win_height() / 2) - (self.rect.h);
		firing_weapon = 0;
		hp = STARTING_HP;
		armor = STARTING_ARMOR;
	}
	wpn::MP5 mp5;
	Actor self;
	int movement_amount;
	int cx;
	int cy;
	int angle;
	bool ready;
	bool firing_weapon;
	int16_t hp;
	int16_t armor;
	bool weapon_should_fire() {
		return mp5.should_fire();
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

	Player() : ready(false) {}
	/** Copy constructor */
	Player(const Player& other) = delete;

	int pixels_per_tick() {
		/**
		 * TODO FIXME: change when a different gun
		 */
		return 20;
	}

	SDL_Texture* initial_texture() {
		return self.bmp[0].texture;
	}
	void calc() {
		cx =  self.rect.x + W / SCALE;
		cy =  self.rect.y + H / SCALE;
	}

};


#if 0
#endif

floatPoint ms_point ;
floatPoint plr_point ;
floatPoint top_right;
floatPoint bot_right;
namespace plr {
	using namespace static_guy;
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
	void rotate_guy() {
		p->angle = coord::get_angle(p->cx,p->cy,cursor::mouse_x,cursor::mouse_y);
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
	}
	void set_guy(Player* g) {
		static_guy::p = g;
	}
	int cx() {
		return static_guy::p->cx;
	}
	int cy() {
		return static_guy::p->cy;
	}
	int get_cx() {
		return static_guy::p->cx;
	}
	int get_cy() {
		return static_guy::p->cy;
	}
	void calc() {
		static_guy::p->calc();
	}
	SDL_Rect* get_rect() {
		return &static_guy::p->self.rect;
	}
	void take_damage(weapon_stats_t * stats) {
		static_guy::p->hp -= rand_between(stats);
		std::cout << "player hp: " << static_guy::p->hp << "\n";
	}

	void redraw_guy() {
		using namespace static_guy;
		if(draw_state::player::draw_guy()) {
			SDL_RenderCopyEx(
			    ren,  //renderer
			    static_guy::p->self.bmp[0].texture,
			    nullptr,// src rect
			    &p->self.rect,
			    p->angle, // angle
			    nullptr,//&center,  // center
			    SDL_FLIP_NONE// flip
			);
		}

	}
	void draw_reticle() {
		if(draw_state::reticle::draw_reticle()) {
			using namespace static_guy;
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
			auto color = GREEN;
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

			SDL_SetRenderDrawColor(ren,r,g,b,a);
		}
	}
};
#endif
