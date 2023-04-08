#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <memory>
#include "hallway.hpp"
#include "world.hpp"
#include "triangle.hpp"
SDL_Texture* ContinuousLTRWall::initial_texture() {
	return self.bmp[0].texture;
}

SDL_Texture* Hallway::initial_texture() {
	return self.bmp[0].texture;
}
Hallway::Hallway(
    Orientation _ori,
    int32_t _x,
    int32_t _y,
    const char* _bmp_path,
    int _w,
    int _h) :
	orientation(_ori),
	self(_x,_y,_bmp_path),
	movement_amount(10),
	ready(true) {
	self.rect.w = _w;
	self.rect.h = _h;
	self.rect.x = _x;
	self.rect.y = _y;
	x = self.rect.x;
	y = self.rect.y;
}
