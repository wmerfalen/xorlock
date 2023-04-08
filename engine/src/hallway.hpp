#ifndef __HALLWAY_HEADER__
#define __HALLWAY_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <memory>
#include "actor.hpp"
//#include "world.hpp"
#include "triangle.hpp"
#include "draw.hpp"
#include "window.hpp"
//extern int tile_width();
//extern void draw_line(int x, int y,int tox,int toy);
enum Orientation : uint16_t {
	HORIZONTAL,
	VERTICAL,
};
struct ContinuousLTRWall {
	enum Version : uint16_t {
		UPPER_WALL,
		LOWER_WALL,
	};
	static constexpr const char* UPPER_WALL_BMP = "../assets/hallway-ltr-wall-%d.bmp";
	static constexpr const char* LOWER_WALL_BMP = "../assets/hallway-ltr-wall-%d.bmp";

	ContinuousLTRWall(Version _vrsn,int _x,int _y) :
		version(_vrsn),
		self(_x,_y,(version == UPPER_WALL ? UPPER_WALL_BMP : LOWER_WALL_BMP)),
		movement_amount(10),
		ready(true) {
		self.rect.w = 250;
		self.rect.h = 400;
		self.rect.x = (win_width() / 4) - (self.rect.w / 4);
		self.rect.y = (win_height() / 4) - (self.rect.h / 4);
		x = self.rect.x;
		y = self.rect.y;
	}
	Version version;
	Actor self;
	int movement_amount;
	bool ready;
	int& x = self.rect.x;
	int& y = self.rect.y;

	ContinuousLTRWall() = delete;
	/** Copy constructor */
	ContinuousLTRWall(const ContinuousLTRWall& other) = delete;

	SDL_Texture* initial_texture();

};

struct Hallway {
	Hallway(Orientation _ori,int32_t _x,int32_t _y,const char* _bmp_path, int _w, int _h);
	Orientation orientation;
	Actor self;
	int movement_amount;
	bool ready;
	int& x = self.rect.x;
	int& y = self.rect.y;

	Hallway() = delete;
	/** Copy constructor */
	Hallway(const Hallway& other) = delete;

	SDL_Texture* initial_texture();

};


#endif
