#include <SDL2/SDL.h>
#include <iostream>
#include "circle.hpp"
#include "extern.hpp"

#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";

namespace shapes {
	void DrawCircle(int32_t centreX, int32_t centreY, int32_t radius) {
		const int32_t diameter = (radius * 2);

		int32_t x = (radius - 1);
		int32_t y = 0;
		int32_t tx = 1;
		int32_t ty = 1;
		int32_t error = (tx - diameter);

		while(x >= y) {
			//  Each of the following renders an octant of the circle
			SDL_RenderDrawPoint(ren, centreX + x, centreY - y);
			SDL_RenderDrawPoint(ren, centreX + x, centreY + y);
			SDL_RenderDrawPoint(ren, centreX - x, centreY - y);
			SDL_RenderDrawPoint(ren, centreX - x, centreY + y);
			SDL_RenderDrawPoint(ren, centreX + y, centreY - x);
			SDL_RenderDrawPoint(ren, centreX + y, centreY + x);
			SDL_RenderDrawPoint(ren, centreX - y, centreY - x);
			SDL_RenderDrawPoint(ren, centreX - y, centreY + x);

			if(error <= 0) {
				++y;
				error += ty;
				ty += 2;
			}

			if(error > 0) {
				--x;
				tx += 2;
				error += (tx - diameter);
			}
		}
	}

	std::vector<SDL_Point> CaptureDrawCircle(const int32_t& centreX, const int32_t& centreY, const int32_t& radius) {
		std::vector<SDL_Point> s;
		const int32_t diameter = (radius * 2);

		int32_t x = (radius - 1);
		int32_t y = 0;
		int32_t tx = 1;
		int32_t ty = 1;
		int32_t error = (tx - diameter);

		while(x >= y) {
			//  Each of the following renders an octant of the circle
			s.emplace_back(centreX + x, centreY - y);
			s.emplace_back(centreX + x, centreY + y);
			s.emplace_back(centreX - x, centreY - y);
			s.emplace_back(centreX - x, centreY + y);
			s.emplace_back(centreX + y, centreY - x);
			s.emplace_back(centreX + y, centreY + x);
			s.emplace_back(centreX - y, centreY - x);
			s.emplace_back(centreX - y, centreY + x);

			if(error <= 0) {
				++y;
				error += ty;
				ty += 2;
			}

			if(error > 0) {
				--x;
				tx += 2;
				error += (tx - diameter);
			}
		}
		return s;
	}
};
