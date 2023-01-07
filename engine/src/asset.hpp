#ifndef __ASSET_HEADER__
#define __ASSET_HEADER__
#include <SDL2/SDL.h>
#include <iostream>

#ifdef FAIL
#undef FAIL
#define FAIL(A) std::cerr << "FAIL(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
#endif

struct Asset {
  SDL_Surface* surface;
  SDL_Texture* texture;
};



#endif
