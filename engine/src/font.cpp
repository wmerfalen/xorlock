#include <iostream>
#include <array>
#include <map>
#include <string>

#include "font.hpp"
#include "draw.hpp"
#include "colors.hpp"
#include "timeline.hpp"

//#define FONT_DEBUG
#ifdef FONT_DEBUG
#define m_debug(A) std::cerr << "[DEBUG]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";
#else
#define m_debug(A)
#endif
namespace font {
	static TTF_Font* font = nullptr;
	static int render_style = TTF_STYLE_NORMAL;
	static int outline = 0;
	static int kerning = 1;
	static int hinting = TTF_HINTING_NORMAL;
	//static SDL_Color white = {0xff,0xff,0xff,0};
	static SDL_Surface *text = nullptr;
	static SDL_Texture *message = nullptr;
	static std::map<std::string,SDL_Texture*> text_textures_map;
	static std::map<std::string,SDL_Surface*> text_surfaces_map;
	namespace white {
		static std::map<std::string,SDL_Texture*> text_textures_map;
		static std::map<std::string,SDL_Surface*> text_surfaces_map;
	};
	namespace red {
		static std::map<std::string,SDL_Texture*> text_textures_map;
		static std::map<std::string,SDL_Surface*> text_surfaces_map;
	};
	namespace green {
		static std::map<std::string,SDL_Texture*> text_textures_map;
		static std::map<std::string,SDL_Surface*> text_surfaces_map;
	};
	void init() {
		TTF_Init();
		font = TTF_OpenFont("../assets/font.ttf",13);
		TTF_SetFontStyle(font, render_style);
		TTF_SetFontOutline(font, outline);
		TTF_SetFontKerning(font, kerning);
		TTF_SetFontHinting(font, hinting);
		queue_characters();
	}
	void quit() {
		for(const auto& pair : red::text_surfaces_map) {
			SDL_FreeSurface(pair.second);
		}
		for(const auto& pair : white::text_surfaces_map) {
			SDL_FreeSurface(pair.second);
		}
		for(const auto& pair : green::text_surfaces_map) {
			SDL_FreeSurface(pair.second);
		}
		for(const auto& pair : red::text_textures_map) {
			SDL_DestroyTexture(pair.second);
		}
		for(const auto& pair : white::text_textures_map) {
			SDL_DestroyTexture(pair.second);
		}
		for(const auto& pair : green::text_textures_map) {
			SDL_DestroyTexture(pair.second);
		}
		TTF_Quit();
	}
	void queue_characters() {
		std::string chars = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@#$%^&*()_+-=\"',./<>?`~[]";
		std::string tmp;
		for(const auto& ch : chars) {
			tmp.clear();
			tmp += ch;
      if(white::text_surfaces_map.find(tmp) != white::text_surfaces_map.end()){
        SDL_FreeSurface(white::text_surfaces_map[tmp]);
      }
			white::text_surfaces_map[tmp] = TTF_RenderText_Solid(font,tmp.c_str(),colors::sdl::white());
			white::text_textures_map[tmp] = SDL_CreateTextureFromSurface(ren,white::text_surfaces_map[tmp]);
		}
		for(const auto& ch : chars) {
			tmp.clear();
			tmp += ch;
      if(red::text_surfaces_map.find(tmp) != red::text_surfaces_map.end()){
        SDL_FreeSurface(red::text_surfaces_map[tmp]);
      }
			red::text_surfaces_map[tmp] = TTF_RenderText_Solid(font,tmp.c_str(),colors::sdl::red());
			red::text_textures_map[tmp] = SDL_CreateTextureFromSurface(ren,red::text_surfaces_map[tmp]);
		}
		for(const auto& ch : chars) {
			tmp.clear();
			tmp += ch;
			if(green::text_surfaces_map.find(tmp) != green::text_surfaces_map.end()){
        SDL_FreeSurface(green::text_surfaces_map[tmp]);
      }
			green::text_surfaces_map[tmp] = TTF_RenderText_Solid(font,tmp.c_str(),colors::sdl::green());
			green::text_textures_map[tmp] = SDL_CreateTextureFromSurface(ren,green::text_surfaces_map[tmp]);
		}
	}
	void small_white_text(const SDL_Point* where,const std::string& msg,int height){
		int i = 0;
		std::string tmp;
    int calc_width = height * 0.75;
		for(const auto& ch : msg) {
			tmp.clear();
			tmp += ch;
			SDL_Rect r{where->x + (i * calc_width),where->y,height,calc_width};
			SDL_RenderCopy(ren,white::text_textures_map[tmp],nullptr,&r);
			++i;
		}
	}
	void small_green_text(const SDL_Point* where,const std::string& msg,int height){
		int i = 0;
		std::string tmp;
    int calc_width = height * 0.75;
		for(const auto& ch : msg) {
			tmp.clear();
			tmp += ch;
			SDL_Rect r{where->x + (i * calc_width),where->y,height,calc_width};
			SDL_RenderCopy(ren,green::text_textures_map[tmp],nullptr,&r);
			++i;
		}
	}
	void small_red_text(const SDL_Point* where,const std::string& msg,int height){
		int i = 0;
		std::string tmp;
    int calc_width = height * 0.75;
		for(const auto& ch : msg) {
			tmp.clear();
			tmp += ch;
			SDL_Rect r{where->x + (i * calc_width),where->y,height,calc_width};
			SDL_RenderCopy(ren,red::text_textures_map[tmp],nullptr,&r);
			++i;
		}
	}
	void red_text(const SDL_Point* where,const std::string& msg,const uint16_t& height, const uint16_t& width) {
		int i = 0;
		int calc_width = width / msg.length();
		std::string tmp;
		for(const auto& ch : msg) {
			tmp.clear();
			tmp += ch;
			SDL_Rect r{where->x + (i * calc_width),where->y,height,calc_width};
			SDL_RenderCopy(ren,red::text_textures_map[tmp],nullptr,&r);
			++i;
		}
	}
	void white_text(const SDL_Point* where,const std::string& msg,const uint16_t& height, const uint16_t& width) {
		int i = 0;
		int calc_width = width / msg.length();
		std::string tmp;
		for(const auto& ch : msg) {
			tmp.clear();
			tmp += ch;
			SDL_Rect r{where->x + (i * calc_width),where->y,height,calc_width};
			SDL_RenderCopy(ren,white::text_textures_map[tmp],nullptr,&r);
			++i;
		}
	}
	void green_text(const SDL_Point* where,const std::string& msg,const uint16_t& height, const uint16_t& width) {
		int i = 0;
		int calc_width = width / msg.length();
		std::string tmp;
		for(const auto& ch : msg) {
			tmp.clear();
			tmp += ch;
			SDL_Rect r{where->x + (i * calc_width),where->y,height,calc_width};
			SDL_RenderCopy(ren,green::text_textures_map[tmp],nullptr,&r);
			++i;
		}
	}
	void draw_bubble_text_once(const SDL_Point* where,const std::string& msg,uint16_t size,const SDL_Color& color) {
		auto text = TTF_RenderText_Solid(font, msg.c_str(),color);
		auto message = SDL_CreateTextureFromSurface(ren,text);
		SDL_FreeSurface(text);
		SDL_Rect r{where->x,where->y,size,size};
		SDL_RenderCopy(ren,message,nullptr,&r);
    SDL_DestroyTexture(message);
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
    SDL_DestroyTexture(message);
	}
	void draw_bubble_text(const SDL_Point* where,const std::string& msg) {
		return draw_bubble_text(where,msg,95,colors::sdl::white());
	}
	void draw_bubble_text(const SDL_Point* where,const std::string& msg,uint16_t size) {
		return draw_bubble_text(where,msg,size,colors::sdl::white());
	}
};
