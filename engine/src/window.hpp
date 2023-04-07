#ifndef __WINDOW_HEADER__
#define __WINDOW_HEADER__
#include <iostream>
#include <SDL2/SDL.h>

int win_width();
int win_height();
int tile_width();
void ren_clear();
void ren_present();
extern SDL_Renderer* ren;


#endif
