#ifndef __CIRCLE_HEADER__
#define __CIRCLE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include "actor.hpp"


#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";

namespace shapes {
	void DrawCircle(int32_t centreX, int32_t centreY, int32_t radius);

	std::vector<SDL_Point> CaptureDrawCircle(const int32_t& centreX, const int32_t& centreY, const int32_t& radius);
};

#undef LD

#endif
