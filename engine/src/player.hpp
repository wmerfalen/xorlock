#ifndef __PLAYER_HEADER__
#define __PLAYER_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <memory>
#include "actor.hpp"
#include "world.hpp"

struct Player {
  Player(int32_t _x,int32_t _y,const char* _bmp_path) : 
    self(_x,_y,_bmp_path),
    ready(true) {
    }
  Actor self;
  bool ready;

  Player() : ready(false) {}
  /** Copy constructor */
  Player(const Player& other) = delete;

  void wants_to_move(Direction dir) {

  }
};


#endif
