#include <SDL2/SDL.h>
#include <iostream>
#include "triangle.hpp"

#ifdef FAIL
#undef FAIL
#endif

#ifdef DEBUG
#undef DEBUG
#endif

#define FAIL(A) std::cerr << "FAIL(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
#define DEBUG(A) std::cout << "DEBUG(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
extern SDL_Renderer* ren;

namespace math {

};
