#ifndef __WINDOW_HEADER__
#define __WINDOW_HEADER__
#include <iostream>
#include <array>
#include <string>
#include <SDL2/SDL.h>

namespace window::resolution {
  struct modes_t {
    uint16_t WIDTH;
    uint16_t HEIGHT;
    std::string description;
    float units;
    std::size_t initial_points;
  };
  extern std::array<modes_t,4> modes;
};

int win_width();
int win_height();
int tile_width();
void ren_clear();
void ren_present();
extern SDL_Renderer* ren;


#endif
