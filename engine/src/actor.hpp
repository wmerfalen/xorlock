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
	Actor(int32_t _x,int32_t _y,const char* _bmp_path);
	~Actor() = default;
	int x;
	int y;
  int z;
	int cx;
	int cy;
	void calc();
	int world_x;
	int world_y;

	std::vector<Asset> bmp;
  Asset* random_bmp();
	SDL_Rect rect;
	bool ready;

  void free_existing();
	Actor();
	/** Copy constructor */
	Actor(const Actor& other);
	std::string report() const;
};


void actor_program_exit();
#endif
