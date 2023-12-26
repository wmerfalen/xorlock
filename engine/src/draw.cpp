#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "player.hpp"
#include <iostream>
#include <array>

#include "draw.hpp"
#include "colors.hpp"
#include "font.hpp"
#include "wall.hpp"
#include "npc/paths.hpp"

// FIXME
//#define DRAW_GATEWAYS
//#define DRAW_OVERLAY_GRID

namespace draw {
	std::vector<SDL_Rect> blatant_list;
	static constexpr SDL_Color green = {0,255,0,0};
	static constexpr SDL_Color red = {255,0,0,0};
	static constexpr SDL_Color white = {255,255,255,0};
	static constexpr SDL_Color grey = {0xee,0xee,0xee,99};
	void erase_timeline() {
	}
	static uint8_t rotation = 0;
	static uint16_t size = 30;
	static const auto color = colors::green();
	static SDL_Rect r;
  static float w_s= 1.0;
  static float h_s= 1.0;
  static float accel = 0.009;
  static int target_width = 500;
  static int target_height = 100;
  void draw_iteration(float w_scale,float h_scale){
#if 0
		r.x = plr::cx() - 400;
		r.y = plr::cy() + 400;
		r.w = size * w_scale;
    if(!(abs(r.h) >= target_height)){
		  r.h = size * h_scale;
    }
    if(abs(r.w) >= target_width){
      r.w = target_width;
    }
		switch(rotation) {
			case 1:
				r.x += 20;
				r.y += 20;
				break;
			case 2:
				r.y += 40;
				break;
			case 3:
				r.x -= 20;
				r.y += 20;
				break;
			default:
				break;
		}
		SDL_RenderDrawRect(ren,&r);
#endif
  }
  float accellerator(){
    accel += 0.091;
    return accel;
  }
	void tick_timeline() {
#if 0
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
    w_s += 0.01 * accellerator();
    h_s -= 0.01 * accellerator();

    for(uint8_t i=0; i < 4;i++){
      rotation = i;
      draw_iteration(w_s,h_s);
    }
    for(uint8_t i=4; i > 0;i--){
      rotation = i;
      draw_iteration(w_s,h_s);
    }
		restore_draw_color();
#endif
	}
	void grey_letter_at(const SDL_Rect* where,const std::string& _msg,const uint16_t& size) {
		SDL_Point p;
		p.x=where->x;
		p.y=where->y;
		font::draw_bubble_text(&p,_msg,size,grey);

	}
	void red_letter_at(const SDL_Rect* where,const std::string& _msg,const uint16_t& size) {
		SDL_Point p;
		p.x=where->x;
		p.y=where->y;
		font::draw_bubble_text(&p,_msg,size,red);
	}
	void green_letter_at(const SDL_Rect* where,const std::string& _msg,const uint16_t& size) {
		SDL_Point p;
		p.x=where->x;
		p.y=where->y;
		font::draw_bubble_text(&p,_msg,size,green);
	}
	void letter_at(const SDL_Rect* where,const std::string& _msg,const uint16_t& size) {
		SDL_Point p;
		p.x=where->x;
		p.y=where->y;
		font::draw_bubble_text(&p,_msg,size,white);
	}
	void bubble_text(const SDL_Point* where,const std::string& _msg) {
		font::draw_bubble_text(where,_msg,95,white);
	}
	void letter_at(const SDL_Point* where,const std::string& _msg) {
		font::draw_bubble_text(where,_msg,95,white);
	}
	void letter_at(const SDL_Rect* _where,const std::string& _msg) {
		SDL_Point p;
		p.x = _where->x;
		p.y = _where->y;
		font::draw_bubble_text(&p,_msg,95,white);
	}
	void hires_line(const SDL_Point* from, const SDL_Point* to) {
		static const auto color = colors::bullet_line();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		SDL_RenderDrawLine(ren,
		                   from->x,
		                   from->y,
		                   to->x,
		                   to->y
		                  );
		restore_draw_color();

	}
	void blatant() {
		static int calls = 0;
		++calls;
		for(auto& r : blatant_list) {
			blatant_rect(&r);
		}
		if(calls >= 30) {
			calls = 0;
			blatant_list.clear();
		}
	}
	void register_blatant(SDL_Rect* r) {
		blatant_list.emplace_back(*r);
	}
	void blatant_rect(SDL_Rect* r) {
		static const auto color = colors::green();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		SDL_RenderDrawRect(ren, r);
		SDL_Rect copy_a = *r;
		for(int i=0; i < 10; i++) {
			copy_a.w += 2;
			copy_a.h += 2;
			copy_a.x -= 1;
			copy_a.y -= 1;
			SDL_RenderDrawRect(ren, &copy_a);
		}
		restore_draw_color();
	}
	void fill_rect(SDL_Rect* r,uint8_t color[]) {
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		SDL_RenderDrawRect(ren, r);
		SDL_Rect copy_a = *r;
		for(int i=0; i < 10; i++) {
			copy_a.w += 2;
			copy_a.h += 2;
			copy_a.x -= 1;
			copy_a.y -= 1;
			SDL_RenderDrawRect(ren, &copy_a);
		}
    SDL_RenderFillRect(ren,r);
		restore_draw_color();
	}
	//static int angle_offset = 0;
	void axis(int angle,uint8_t color[]) {
		uint8_t r,g,b,a;
		SDL_GetRenderDrawColor(ren,&r,&g,&b,&a);
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		int x = plr::cx();
		int y = plr::cy();
		SDL_RenderDrawLine(ren,
		                   x,
		                   y,
		                   (512) * cos(angle),
		                   (512) * sin(angle)
		                  );
		SDL_SetRenderDrawColor(ren,r,g,b,a);
	}
	void rect(SDL_Rect* r) {
		static const auto color = colors::green();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		SDL_RenderDrawRect(ren, r);
		restore_draw_color();
	}
	void grid() {
#if defined(DRAW_OVERLAY_GRID) || defined(DRAW_GATEWAYS)
		static const auto color = colors::green();
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
#endif
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
	void overlay_grid() {
    /**
     * This function _ALSO_ draws the gateways used by npc/paths.cpp
     */
#if defined(DRAW_OVERLAY_GRID) || defined(DRAW_GATEWAYS)
		static const auto color = colors::green();
		static const auto red_color = colors::red();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		for(const auto& w : wall::walls) {
			SDL_RenderDrawRect(ren,&w->rect);
		}
		SDL_SetRenderDrawColor(ren,red_color[0],red_color[1],red_color[2],0);
		for(const auto& line : npc::paths::demo_points) {
			if(std::get<1>(line)) {
				SDL_Rect r;
				r.x = std::get<0>(line).first;
				r.y = std::get<0>(line).second;
				r.w = 30;//CELL_WIDTH;
				r.h = 30;//CELL_HEIGHT;
        SDL_RenderFillRect(ren,&r);
				SDL_RenderDrawRect(ren,&r);
			}
		}
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		for(const auto& line : npc::paths::gw_points) {
			if(std::get<1>(line)) {
				SDL_Rect r;
				r.x = std::get<0>(line).first;
				r.y = std::get<0>(line).second;
				r.w = 30;//CELL_WIDTH;
				r.h = 30;//CELL_HEIGHT;
        SDL_RenderFillRect(ren,&r);
				SDL_RenderDrawLine(ren,
				                   r.x,
				                   r.y,
				                   plr::self()->rect.x,
				                   plr::self()->rect.y
				                  );
			}
		}
		restore_draw_color();
#endif
	}
	void red_dot(const SDL_Rect* r) {
		static const auto color = colors::red();
		save_draw_color();
		SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
		SDL_Rect f = *r;
		f.w = 10;
		f.h = 10;
		SDL_RenderDrawRect(ren,&f);
	}
};
