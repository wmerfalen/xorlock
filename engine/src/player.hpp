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
	Player(int32_t _x,int32_t _y,const char* _bmp_path) :
		self(_x,_y,_bmp_path),
		movement_amount(10),
		ready(true) {
		self.rect.w = W;
		self.rect.h = H;
		self.rect.x = (win_width() / 2) - (self.rect.w);
		self.rect.y = (win_height() / 2) - (self.rect.h);
	}
	wpn::MP5 mp5;
	Actor self;
	int movement_amount;
	int cx;
	int cy;
	int angle;
	bool ready;
	int gun_damage() {
		return rand_between(GUN_DAMAGE_RANDOM_LO,GUN_DAMAGE_RANDOM_HI);
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


int angle_offset = 0;
void draw_axis(Player& p,int angle,uint8_t color[]) {
	uint8_t r,g,b,a;
	SDL_GetRenderDrawColor(ren,&r,&g,&b,&a);
	SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
	int x = p.cx;
	int y = p.cy;
	SDL_RenderDrawLine(ren,
	                   x,
	                   y,
	                   (512) * cos(angle + angle_offset),
	                   (512) * sin(angle + angle_offset)
	                  );
	SDL_SetRenderDrawColor(ren,r,g,b,a);
}
void draw_line(int x, int y,int tox,int toy) {
	static const auto color = GREEN;
	save_draw_color();
	SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
	SDL_RenderDrawLine(ren,
	                   x,
	                   y,
	                   tox,
	                   toy
	                  );
	restore_draw_color();
}

void draw_grid() {
	static const auto color = GREEN;
	save_draw_color();
	SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
	std::vector<SDL_Point> points;
	bool bruh = true;
	for(int x=0; x <= win_width(); x += tile_width()) {
		if(bruh) {
			points.emplace_back(x,win_height());
			points.emplace_back(x + tile_width(),win_height());
		} else {
			points.emplace_back(x,0);
			points.emplace_back(x,win_height());
		}
		bruh = !bruh;
	}
	points.emplace_back(win_width(),win_height());
	points.emplace_back(win_width(),0);
	points.emplace_back(0,0);

	bruh = true;
	for(int y=0; y <= win_height() + tile_width(); y += tile_width()) {
		//----------------------------
		points.emplace_back(0,y);   //|
		//                            |
		//                           \|/
		points.emplace_back(win_width(),y);
		points.emplace_back(win_width(),y + tile_width());
		//<---------------------------/
		points.emplace_back(0,y + tile_width());
	}
	SDL_RenderDrawLines(ren,
	                    &points[0],
	                    points.size()
	                   );
	restore_draw_color();
}


floatPoint ms_point ;
floatPoint plr_point ;
floatPoint top_right;
floatPoint bot_right;
namespace plr {
	static bool firing_gun;
	using namespace static_guy;
	void start_gun(const int& mouse_x,const int& mouse_y) {
		p->calc();
		firing_gun = true;
		travel_to(mouse_x,mouse_y);
	}
	void stop_gun() {
		firing_gun = false;
	}
	bool is_firing() {
		return firing_gun;
	}
	void fire_weapon() {
		p->calc();
		if(p->mp5.should_fire()) {
			for(uint8_t i=0; i < p->mp5.burst(); ++i) {
				travel_to(cursor::mouse_x,cursor::mouse_y);
			}
		}
	}
	void rotate_guy(Player& p,const int& mouse_x,const int& mouse_y) {
		p.calc();
		p.angle = coord::get_angle(p,mouse_x,mouse_y);
		SDL_RenderCopyEx(
		    ren,  //renderer
		    p.self.bmp[0].texture,
		    nullptr,// src rect
		    &p.self.rect,
		    p.angle, // angle
		    nullptr,  // center
		    SDL_FLIP_NONE // flip
		);
	}
	void set_guy(Player* g) {
		static_guy::p = g;
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

	void redraw_guy() {
		using namespace static_guy;
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
};
int measure_distance(const Point& start,const SDL_Rect& target) {
	return sqrt(pow(target.x - start.x,2) - pow(target.y - start.y,2) * 1.0);
}

std::vector<Actor*> npcs_hit_by_bullet(Line& line) {
	std::vector<Actor*> n;
	for(const auto& p : line.getPoints(1024)) {
		SDL_Rect bullet;
		bullet.x = p.x;
		bullet.y = p.y;
		bullet.w = 10;
		bullet.h = 10;
		for(auto& npc : world->npcs) {
			if(SDL_HasIntersection(&bullet,&npc->rect) == SDL_TRUE) {
#ifdef SHOW_COLLISIONS
				std::cout << "HIT\n";
#endif
				n.emplace_back(npc);
			}
		}
	}
	return n;
}

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
	std::vector<Actor*> npcs_hit;
	Travelers() : done(true) {}
	void travel_angle(int _mouse_x,int _mouse_y) {
		using namespace static_guy;
		p->calc();
		pixels_per_tick = p->pixels_per_tick();
		static constexpr double PI = 3.14159265358979323846;
		current_x = p->cx;
		current_y = p->cy;
		line.angle = p->angle = coord::get_angle(*p,_mouse_x,_mouse_y);

		x = 99500 * cos(PI * 2  * p->angle / 360);
		y = 99500 * sin(PI * 2 * p->angle / 360);
#ifdef DEBUG_GUN_LINE
		draw_line(current_x,current_y,x,y);
#endif
		line.p1 = Point {current_x,current_y};
		line.p2 = Point{x,y};
		npcs_hit = npcs_hit_by_bullet(line);

		if(npcs_hit.size()) {
			for(auto& n : npcs_hit) {
				npc::take_damage(n,p->gun_damage());
			}
		}
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
static std::array<Travelers,BULLET_POOL_SIZE> travel_list;
static std::size_t tlist_ctr = 0;
static std::size_t process = 0;

namespace bullet {
	SDL_Rect bullet_rect;
	SDL_Texture* bullet_texture = nullptr;
	SDL_Surface* bullet_surface = nullptr;
};
namespace static_guy {
	void init() {
		srand(time(nullptr));
		using namespace bullet;
		process = 0;
		bcom.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
	}
};


void travel_to(const int& x,const int& y) {
	using namespace static_guy;
	++process;
	if(tlist_ctr >= BULLET_POOL_SIZE) {
		tlist_ctr = 0;
	}
	travel_list[tlist_ctr].travel_angle(x,y);
	++tlist_ctr;
}
static constexpr int BW = 10;
static constexpr int BH = 10;
void fire_tick() {
	using namespace static_guy;
	if(process == 0) {
		return;
	}
	for(std::size_t i=0; i < BULLET_POOL_SIZE; i++) {
		auto& t = travel_list[i];
		if(t.done) {
			continue;
		}
		const auto& point = t.next_point();
		if(t.done) {
			continue;
		}
		t.current_y = round(point.y);
		t.current_x = round(point.x);
		using namespace bullet;
		using namespace static_guy;
		bullet_rect.x = t.current_x;
		bullet_rect.y = t.current_y;
		bullet_rect.w = BW;
		bullet_rect.h = BH;
		SDL_RenderCopy(
		    ren,
		    bcom.bmp[0].texture,
		    nullptr,
		    &bullet_rect
		);
	}
}

void draw_reticle(Player& p,const int& mouse_x,const int& mouse_y) {
	save_draw_color();
	set_draw_color("red");
#ifdef DRAW_SHIELD
	DrawCircle(p.cx,p.cy,W2);
#endif
	restore_draw_color();
#ifdef DRAW_RETICLE
	uint8_t r,g,b,a;
	SDL_GetRenderDrawColor(ren,&r,&g,&b,&a);
	SDL_SetRenderDrawColor(ren,255,0,0,0);
	SDL_RenderDrawLine(ren,
	                   p.cx,
	                   p.cy,
	                   mouse_x,
	                   mouse_y);
	auto color = GREEN;
	SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
	SDL_RenderDrawLine(ren,
	                   p.cx,
	                   p.cy,
	                   top_right.x,
	                   top_right.y
	                  );
	/**
	 * Draw a line between (cx,0)
	 */
	/* Draw line up to north */
	draw_line(p.cx,p.cy,p.cx,0);
	// draw line right to east
	draw_line(p.cx,p.cy,win_width(),p.cy);
	// draw line down to south
	draw_line(p.cx,p.cy,p.cx,win_height());
	// draw line left to west
	draw_line(p.cx,p.cy,0,p.cy);

	SDL_SetRenderDrawColor(ren,r,g,b,a);
#endif
}
#endif
