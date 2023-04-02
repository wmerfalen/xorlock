#ifndef __ACTOR_HEADER__
#define __ACTOR_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <array>

#include "asset.hpp"

#ifdef FAIL
#undef FAIL
#endif

#ifdef DEBUG
#undef DEBUG
#endif

#define FAIL(A) std::cerr << "FAIL(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
#define DEBUG(A) std::cout << "DEBUG(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
extern SDL_Renderer* ren;

struct Actor {
	std::pair<std::size_t,std::size_t> load_bmp_assets(const char* _bmp_path, std::size_t _bmp_count, std::size_t increment);
	bool load_bmp_asset(const char* _bmp_path);
	std::pair<std::size_t,std::size_t> load_bmp_assets(const char* _bmp_path, std::size_t _bmp_count);
	Actor(int32_t _x,int32_t _y,const char* _bmp_path) :
		x(_x),
		y(_y),
		rect({_x,_y,68,38}),
	ready(true) {
		if(std::string(_bmp_path).find_first_of("%d") != std::string::npos) {
			load_bmp_assets(_bmp_path,360);
		} else {
			auto p = SDL_LoadBMP(_bmp_path);
			if(p == nullptr) {
				FAIL("Couldn't load asset: '" << _bmp_path << "'");
				return;
			}
			auto tex = SDL_CreateTextureFromSurface(ren, p);
			bmp.emplace_back(p,tex);
			DEBUG("Loaded '" << _bmp_path << "'");
		}
	}
	~Actor();
	int x;
	int y;
	int cx;
	int cy;
	void calc();

	std::vector<Asset> bmp;
	SDL_Rect rect;
	bool ready;

	Actor() : ready(false) {}
	/** Copy constructor */
	Actor(const Actor& other);
	std::string report() const;
};


#endif
