#ifndef __TRIANGLE_HEADER__
#define __TRIANGLE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>

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

	template <typename TFPoint>
	static inline float sign(TFPoint p1, TFPoint p2, TFPoint p3) {
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	template <typename TFPoint>
	static inline bool PointInTriangle(TFPoint pt, TFPoint v1, TFPoint v2, TFPoint v3) {
		float d1, d2, d3;
		bool has_neg, has_pos;

		d1 = sign(pt, v1, v2);
		d2 = sign(pt, v2, v3);
		d3 = sign(pt, v3, v1);

		has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

		return !(has_neg && has_pos);
	}
};

#endif
