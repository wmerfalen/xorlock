#ifndef __ACTOR_HEADER__
#define __ACTOR_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>


struct Actor {
  Actor(int32_t _x,int32_t _y,const char* _bmp_path) : 
    x(_x),
    y(_y),
    rect({_x,_y,80,80}),
  ready(true){
    bmp = SDL_LoadBMP(_bmp_path);
    }
  ~Actor() {
    if(ready){
      SDL_FreeSurface(bmp);
    }
  }
  int32_t x;
  int32_t y;
  SDL_Surface* bmp;
  SDL_Rect rect;
  bool ready;

  Actor() : bmp(nullptr), ready(false) {}
  /** Copy constructor */
  Actor(const Actor& other){
    x = other.x;
    y = other.y;
    bmp = other.bmp;
  }
};


#endif
