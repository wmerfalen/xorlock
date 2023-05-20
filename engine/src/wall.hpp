#ifndef __WALL_HEADER__
#define __WALL_HEADER__

#include "actor.hpp"
#include "world.hpp"
#include "coordinates.hpp"
#include "viewport.hpp"
#include "circle.hpp"
#include "rng.hpp"
#include "draw.hpp"
#include <vector>
#include <functional>
#include <memory>
#include "direction.hpp"

namespace wall {
	enum Texture : int32_t {
		BR_TWALL = 25,
		BR_BWALL = 41,
		BR_TR_CRNR= 26, /** Top right corner of cement border */
		BR_BR_CRNR = 42,  /** bottom right corner of cement border */
		BR_TL_CRNR = 24, /** Top left corner of cement border */
		BR_BL_CRNR = 40, /** Bottom left corner of cement border */
		BLD_TWALL = 1,
		BLD_BWALL = 17,
		BLD_RWALL = 10,
		BLD_LWALL = 8,
		BLD_TR_CRNR = 2,
		BLD_BR_CRNR = 18,
		BLD_TL_CRNR = 0,
		BLD_BL_CRNR = 16,
		BLD_INNR_TL_CRNR = 9,
		DIRT = 14,
		DIRTY_BUSH = 38,
		EMPTY = -1,
		GRASS = 39,
	};
	std::string to_string(Texture t);
	static const std::vector<Texture> WALKABLE{
		DIRT,
		DIRTY_BUSH,
		EMPTY,
		GRASS,
	};
	static std::vector<Texture> TEXTURES{
		Texture::EMPTY,
		Texture::BR_TWALL,
		Texture::BR_BWALL,
		Texture::BR_TR_CRNR,
		Texture::BR_BR_CRNR,
		Texture::BR_TL_CRNR,
		Texture::BR_BL_CRNR,
		Texture::GRASS,
		Texture::BLD_TWALL,
		Texture::BLD_BWALL,
		Texture::BLD_RWALL,
		Texture::BLD_LWALL,
		Texture::BLD_TR_CRNR,
		Texture::BLD_BR_CRNR,
		Texture::BLD_TL_CRNR,
		Texture::BLD_BL_CRNR,
		Texture::BLD_INNR_TL_CRNR,
		Texture::DIRT,
		Texture::DIRTY_BUSH,
	};
	struct Wall {
		bool is_gateway;
		Texture type;
		SDL_Rect rect;
		bool initialized;
		bool walkable;
		Wall(
		    const int& _x,
		    const int& _y,
		    const int& _width,
		    const int& _height,
		    Texture _type);
		Wall() : is_gateway(false), initialized(false) {}
		Wall(const Wall& o) = delete;
		~Wall() = default;
		void render();
		template <typename T>
		bool any_of(const T& t) const {
			for(const auto& tx : t) {
				if(type == tx) {
					return true;
				}
			}
			return false;
		}
	};// end Wall
	bool can_move(int direction,int amount);
	void move_map(int direction,int amount);
	void tick();
	void init();
	void draw_wall_at(
	    const int& _x,
	    const int& _y,
	    const int& _width,
	    const int& _height,
	    Texture _type
	);

	struct Remedy {
		/**
		 * If you're blocked from going "blocked_direction"...
		 */
		Direction blocked_direction;
		/**
		 * And this span of wall objects are preventing you from
		 * going "blocked_direct"...
		 */
		std::vector<wall::Wall*> obstacles;
		/**
		 * A walkable area exists to the "escape_direction"
		 * of the following "edge" wall object
		 */
		Direction escape_direction;
		wall::Wall* edge;
	};

	extern std::vector<std::unique_ptr<Wall>> walls;
	extern std::vector<Wall*> blockable_walls;
	extern std::vector<Wall*> walkable_walls;
	extern std::vector<std::unique_ptr<Remedy>> path_remedies;
	extern std::vector<Wall*> gateways;
};

#endif
