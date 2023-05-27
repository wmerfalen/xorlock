#include <iostream>
#include <array>
#include <map>
#include <string>

#include "font.hpp"
#include "draw.hpp"
#include "colors.hpp"
#include "timeline.hpp"

namespace font {
	static TTF_Font* font = nullptr;
	static int render_style = TTF_STYLE_NORMAL;
	static int outline = 0;
	static int kerning = 1;
	static int hinting = TTF_HINTING_NORMAL;
	static SDL_Color white = {0xff,0xff,0xff,0};
	static SDL_Surface *text = nullptr;
	static SDL_Texture *message = nullptr;
	static std::map<std::string,SDL_Texture*> text_textures_map;
	static std::map<std::string,SDL_Surface*> text_surfaces_map;
	void init() {
		TTF_Init();
		font = TTF_OpenFont("/home/xkold/progs/font.ttf",9);
		TTF_SetFontStyle(font, render_style);
		TTF_SetFontOutline(font, outline);
		TTF_SetFontKerning(font, kerning);
		TTF_SetFontHinting(font, hinting);
	}
	void quit() {
		TTF_Quit();
	}
	void draw_bubble_text(const SDL_Point* where,const std::string& msg,uint16_t size,const SDL_Color& color) {
		bool allocated_text = false;
		if(text_surfaces_map.find(msg) != text_surfaces_map.end()) {
			text = text_surfaces_map[msg];
		} else {
			text = TTF_RenderText_Solid(font, msg.c_str(),color);
			text_surfaces_map[msg] = text;
			allocated_text = true;
		}
		if(text == nullptr) {
			return;
		}
		if(text_textures_map.find(msg) != text_textures_map.end()) {
			message = text_textures_map[msg];
		} else {
			message = SDL_CreateTextureFromSurface(ren,text);
			text_textures_map[msg] = message;
		}
		if(allocated_text) {
			SDL_FreeSurface(text);
		}
		SDL_Rect r{where->x,where->y,size,size};
		SDL_RenderCopy(ren,message,nullptr,&r);
	}
	void draw_bubble_text(const SDL_Point* where,const std::string& msg) {
		return draw_bubble_text(where,msg,95,white);
	}
	void draw_bubble_text(const SDL_Point* where,const std::string& msg,uint16_t size) {
		return draw_bubble_text(where,msg,size,white);
	}
};
