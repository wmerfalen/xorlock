#include <iostream>
#include <array>

#include "font.hpp"
#include "draw.hpp"
#include "color.hpp"

namespace font {
	static TTF_Font* font = nullptr;
	static int render_style = TTF_STYLE_NORMAL;
	static int outline = 0;
	static int kerning = 1;
	static int hinting = TTF_HINTING_NORMAL;
	static SDL_Color white = {0xff,0xff,0xff,0};
	static SDL_Surface *text = nullptr;
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
	void draw_bubble_text(const SDL_Point* where,const std::string& msg) {
		text = TTF_RenderText_Solid(font, msg.c_str(),white);
		if(text == nullptr) {
			std::cerr << "TTF_RenderText_Solid FAILED\n";
			return;
		}
		auto message = SDL_CreateTextureFromSurface(ren,text);
		SDL_FreeSurface(text);
		SDL_Rect r{where->x,where->y,95,95};
		SDL_RenderCopy(ren,message,nullptr,&r);

	}
};
