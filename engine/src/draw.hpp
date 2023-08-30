#ifndef __DRAW_HEADER__
#define __DRAW_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>


namespace draw {
	void erase_timeline();
	void tick_timeline();
	void axis(int angle,uint8_t color[]);
	void grid();
	void bullet_line(int x, int y,int tox,int toy);
	void hires_line(const SDL_Point* from, const SDL_Point* to);
	void draw_green();
	void restore_color();
	void draw_red();
	void rect(SDL_Rect* r);
	void blatant_rect(SDL_Rect* r);
	void blatant();
	void register_blatant(SDL_Rect* r);
	void bubble_text(const SDL_Point* where,const std::string& _msg);
	void letter_at(const SDL_Point* where,const std::string& _msg);
	void letter_at(const SDL_Rect* where,const std::string& _msg);
	void letter_at(const SDL_Rect* where,const std::string& _msg,const uint16_t& size);
	void red_letter_at(const SDL_Rect* where,const std::string& _msg,const uint16_t& size);
	void green_letter_at(const SDL_Rect* where,const std::string& _msg,const uint16_t& size);
	void grey_letter_at(const SDL_Rect* where,const std::string& _msg,const uint16_t& size);
	void red_dot(const SDL_Rect* r);

	void line(int x, int y,int tox,int toy);
	void overlay_grid();
};

#endif
