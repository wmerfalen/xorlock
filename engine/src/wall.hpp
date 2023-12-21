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
#include <set>

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
		START_TILE = 45,
    PORTAL = 30,
    SPAWN_TILE = 46,
    NPC_WAYPOINT_HELPER = 47,
	};
	std::string to_string(Texture t);
	static const std::vector<Texture> WALKABLE{
		DIRT,
		DIRTY_BUSH,
		EMPTY,
		GRASS,
		START_TILE,
    PORTAL,
    SPAWN_TILE,
    NPC_WAYPOINT_HELPER,
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
		Texture::START_TILE,
    Texture::PORTAL,
    Texture::SPAWN_TILE,
    Texture::NPC_WAYPOINT_HELPER,
	};
	struct Wall {
    uint16_t index;
    SDL_Texture* texture;
		bool is_gateway;
		uint8_t* draw_color;
		uint16_t connections;
		Wall* north;
		Wall* north_east;
		Wall* north_west;
		Wall* south;
		Wall* south_east;
		Wall* south_west;
		Wall* east;
		Wall* west;
		int why;
		Texture type;
		SDL_Rect rect;
    SDL_Rect orig_rect;
    Actor* actor_ptr;
		bool initialized;
		bool walkable;
    bool ignore;
		Wall(
		    const int& _x,
		    const int& _y,
		    const int& _width,
		    const int& _height,
		    Texture _type);
		Wall() : texture(nullptr), is_gateway(false), draw_color(nullptr),connections(0), north(nullptr),
			north_east(nullptr), north_west(nullptr), south(nullptr),
			south_east(nullptr), south_west(nullptr), east(nullptr), west(nullptr), why(0), 
      actor_ptr(nullptr),
      initialized(false),
    ignore(true) {}
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
		int32_t distance_to(wall::Wall* other);
		int32_t distance_to(SDL_Rect* other);
    bool build_check();
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

	extern std::vector<std::unique_ptr<Wall>> walls;
	extern std::vector<Wall*> blockable_walls;
	extern std::vector<Wall*> walkable_walls;
	extern std::vector<wall::Wall*> gateways;
	extern std::set<wall::Wall*> blocked;
	wall::Wall* start_tile();
  void program_exit();
};

#endif
