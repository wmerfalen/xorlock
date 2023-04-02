#ifndef __CIRCLE_HEADER__
#define __CIRCLE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include "actor.hpp"
#include "extern.hpp"

#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";

namespace shapes {
	void DrawCircle(int32_t centreX, int32_t centreY, int32_t radius);
	template <typename TActor>
	static inline void surround_actor(TActor& p,const int& mouse_x,const int& mouse_y) {
		save_draw_color();
		set_draw_color("red");
		DrawCircle(p.cx,p.cy,p.self.rect.w);
		restore_draw_color();
	}
	void DrawCircle(int32_t centreX, int32_t centreY, int32_t radius);

	std::vector<SDL_Point> CaptureDrawCircle(const int32_t& centreX, const int32_t& centreY, const int32_t& radius);
};

#undef LD

#endif
