#ifndef __EXTERN_HEADER__
#define __EXTERN_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>
#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";
struct Point {
	double x;
	double y;
};

extern void travel_to(const int& x,const int& y);
struct Line {
	Point p1, p2;
	Line() = default;
	~Line() = default;

	Line(Point p1, Point p2) {
		p1 = p1;
		p2 = p2;
	}
	Line(const Line& o) {
		p1 = o.p1;
		p2 = o.p2;
	}

	std::vector<Point> getPoints(std::size_t quantity) {
		std::vector<Point> points;
		int ydiff = p2.y - p1.y, xdiff = p2.x - p1.x;
		double slope = (double)(p2.y - p1.y) / (p2.x - p1.x);
		double x, y;

		--quantity;

		for(double i = 0; i < quantity; i++) {
			y = slope == 0 ? 0 : ydiff * (i / quantity);
			x = slope == 0 ? xdiff * (i / quantity) : y / slope;
			points.emplace_back(x + p1.x, y + p1.y);
		}

		points[quantity] = p2;
		return points;
	}
	int angle;
};

namespace npc {
	extern void spetsnaz_movement(int);
};
namespace cd {
	extern std::vector<void*> npcs_hit_by_bullet(const Line&);
};
extern int win_width();
extern int win_height();
extern int tile_width();
extern void ren_clear();
extern void ren_present();
extern void save_draw_color();
extern void set_draw_color(const char*);
extern void restore_draw_color();
extern SDL_Renderer* ren;
namespace plr {
	extern int get_cx();
	extern int get_cy();
	extern void calc();
};
namespace saved {
	uint8_t r,g,b,a;
};

uint8_t GREEN[] = {0,255,0};

struct floatPoint {
	float x;
	float y;
};

extern void move_map_by(int,int amount);
extern floatPoint ms_point ;
extern floatPoint plr_point ;
extern floatPoint top_right;
extern floatPoint bot_right;
static inline int rand_between(const int& min,const int& max) {
	return rand()%(max-min + 1) + min;
}

namespace static_guy {
	extern void init();
};

#endif
