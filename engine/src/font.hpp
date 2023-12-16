#ifndef __FONT_HEADER__
#define __FONT_HEADER__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <array>


namespace font {
	void init();
	void quit();
	void draw_bubble_text(const SDL_Point* where,const std::string& msg);
	void draw_bubble_text(const SDL_Point* where,const std::string& msg,uint16_t size);
	void draw_bubble_text(const SDL_Point* where,const std::string& msg,uint16_t size,const SDL_Color& color);
	void draw_bubble_text_once(const SDL_Point* where,const std::string& msg,uint16_t size,const SDL_Color& color);
	void queue_characters();
	void red_text(const SDL_Point* where,const std::string& msg,const uint16_t& height, const uint16_t& width);
	void green_text(const SDL_Point* where,const std::string& msg,const uint16_t& height, const uint16_t& width);
	void white_text(const SDL_Point* where,const std::string& msg,const uint16_t& height, const uint16_t& width);
	void small_red_text(const SDL_Point* where,const std::string& msg,int height);
	void small_green_text(const SDL_Point* where,const std::string& msg,int height);
	void small_white_text(const SDL_Point* where,const std::string& msg,int height);
};

#endif
