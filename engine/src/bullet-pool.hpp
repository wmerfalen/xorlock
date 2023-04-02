#ifndef __BULLET_POOL_HEADER__
#define __BULLET_POOL_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <memory>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
extern int win_width();
extern int win_height();
extern int tile_width();

namespace bullet_pull_unused {
	enum BulletOrientation : uint8_t {
		B_NORTH = 0,
		B_NORTH_EAST,
		B_EAST,
		B_SOUTH_EAST,
		B_SOUTH,
		B_SOUTH_WEST,
		B_WEST,
		B_NORTH_WEST,
		_B_MAX_ = B_NORTH_WEST,
	};
	struct BulletType {
		SDL_Texture* texture;
		BulletOrientation ori;
		uint16_t movement_amount;
		uint16_t type;
	};

	struct BulletPool {
		BulletPool(BulletOrientation _ori,const char* _bmp_path) :
			orientation(_ori),
			movement_amount(10),
			self(0,0,_bmp_path),
			ready(false) {
			calc();
		}
		BulletOrientation orientation;
		int movement_amount;
		int cx;
		int cy;
		std::vector<BulletType> types;
		Actor self;
		bool ready;

		BulletPool();
		/** Copy constructor */
		BulletPool(const BulletPool& other) {
			orientation = other.orientation;
			movement_amount = other.movement_amount;
			self = other.self;
			ready = other.ready;
			calc();
		}

		SDL_Texture* initial_texture() {
			return self.bmp[0].texture;
		}
		void calc() {
			cx = this->self.rect.x + this->self.rect.w / 2;
			cy = this->self.rect.y + this->self.rect.h / 2;
		}

	};
};

#endif
