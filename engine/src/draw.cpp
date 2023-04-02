#ifndef __DRAW_HEADER__
#define __DRAW_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include "extern.hpp"

namespace draw {
	int angle_offset = 0;
	void axis(int angle,uint8_t color[]) {
		uint8_t r,g,b,a;
		SDL_GetRenderDrawColor(ren,&r,&g,&b,&a);
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		int x = plr::cx();
		int y = plr::cy();
		SDL_RenderDrawLine(ren,
		                   x,
		                   y,
		                   (512) * cos(angle + angle_offset),
		                   (512) * sin(angle + angle_offset)
		                  );
		SDL_SetRenderDrawColor(ren,r,g,b,a);
	}
	void grid() {
		static const auto color = GREEN;
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		std::vector<SDL_Point> points;
		bool alternate = true;
		for(int x=0; x <= win_width(); x += tile_width()) {
			if(alternate) {
				points.emplace_back(x,win_height());
				points.emplace_back(x + tile_width(),win_height());
			} else {
				points.emplace_back(x,0);
				points.emplace_back(x,win_height());
			}
			alternate = !alternate;
		}
		points.emplace_back(win_width(),win_height());
		points.emplace_back(win_width(),0);
		points.emplace_back(0,0);

		alternate = true;
		for(int y=0; y <= win_height() + tile_width(); y += tile_width()) {
			//----------------------------
			points.emplace_back(0,y);   //|
			//                            |
			//                           \|/
			points.emplace_back(win_width(),y);
			points.emplace_back(win_width(),y + tile_width());
			//<---------------------------/
			points.emplace_back(0,y + tile_width());
		}
		SDL_RenderDrawLines(ren,
		                    &points[0],
		                    points.size()
		                   );
		restore_draw_color();
	}
	void bullet_line(int x, int y,int tox,int toy) {
		static const auto color = colors::bullet_line();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		SDL_RenderDrawLine(ren,
		                   x,
		                   y,
		                   tox,
		                   toy
		                  );
		restore_draw_color();
	}
	void draw_green() {
		static const auto color = colors::green();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
	}
	void restore_color() {
		restore_draw_color();
	}
	void draw_red() {
		static const auto color = colors::red();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
	}

	void line(int x, int y,int tox,int toy) {
		static const auto color = colors::green();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		SDL_RenderDrawLine(ren,
		                   x,
		                   y,
		                   tox,
		                   toy
		                  );
		restore_draw_color();
	}
};

#endif
