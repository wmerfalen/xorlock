#include "direction.hpp"
#include "wall.hpp"
#include "map.hpp"
#include "world.hpp"
#include "player.hpp"
#include <functional>

namespace wall {
	namespace textures {
		static std::map<Texture,std::unique_ptr<Actor>> map_assets;
	};
	std::vector<std::unique_ptr<Wall>> walls;
	std::string to_string(Texture t) {
		if(t==EMPTY) {
			return "EMPTY";
		}
		if(t==BR_TWALL) {
			return "BR_TWALL";
		}
		if(t==BR_BWALL) {
			return "BR_BWALL";
		}
		if(t==BR_TR_CRNR) {
			return "BR_TR_CRNR";
		}
		if(t==BR_BR_CRNR) {
			return "BR_BR_CRNR";
		}
		if(t==BR_TL_CRNR) {
			return "BR_TL_CRNR";
		}
		if(t==BR_BL_CRNR) {
			return "BR_BL_CRNR";
		}
		if(t==GRASS) {
			return "GRASS";
		}
		if(t==BLD_TWALL) {
			return "BLD_TWALL";
		}
		if(t==BLD_BWALL) {
			return "BLD_BWALL";
		}
		if(t==BLD_RWALL) {
			return "BLD_RWALL";
		}
		if(t==BLD_LWALL) {
			return "BLD_LWALL";
		}
		if(t==BLD_TR_CRNR) {
			return "BLD_TR_CRNR";
		}
		if(t==BLD_BR_CRNR) {
			return "BLD_BR_CRNR";
		}
		if(t==BLD_TL_CRNR) {
			return "BLD_TL_CRNR";
		}
		if(t==BLD_BL_CRNR) {
			return "BLD_BL_CRNR";
		}
		if(t==BLD_INNR_TL_CRNR) {
			return "BLD_INNR_TL_CRNR";
		}
		if(t==DIRT) {
			return "DIRT";
		}
		if(t==DIRTY_BUSH) {
			return "DIRTY_BUSH";
		}

		return "<unknown>";
	}
	SDL_Rect collision;
	Wall::Wall(
	    const int& _x,
	    const int& _y,
	    const int& _width,
	    const int& _height,
	    Texture _type) : type(_type),
		rect{_x,_y,_width,_height} {
		initialized = true;
#ifdef SHOW_WALL_INIT
		std::cout << "rect.x: " << rect.x << "\n";
		std::cout << "rect.y: " << rect.y << "\n";
		std::cout << "rect.w: " << rect.w << "\n";
		std::cout << "rect.h: " << rect.h << "\n";
#endif
		walkable = std::find(WALKABLE.cbegin(),WALKABLE.cend(),type) != WALKABLE.cend();

	}
	void draw_wall_at(
	    const int& _x,
	    const int& _y,
	    const int& _width,
	    const int& _height,
	    Texture _type
	) {
		walls.emplace_back(std::make_unique<Wall>(_x,_y,_width,_height,_type));
	}
	void Wall::render() {
#ifdef NO_WALKABLE_TEXTURES
		if(walkable) {
			return;
		}
#endif
		SDL_RenderDrawRect(ren,&rect);
#ifdef NO_WALL_TEXTURES
#else
		auto ptr = textures::map_assets[type].get();
		if(!ptr || ptr->bmp.size() == 0 || ptr->bmp[0].texture == nullptr) {
			std::cerr << "WARNING: CANNOT RENDER INVALID TEXTURE: " << type << "\n";
			return;
		}
		SDL_RenderCopy(ren, ptr->bmp[0].texture, nullptr, &rect);
#endif
	}
	void tick() {
		draw::draw_green();
		for(auto& wall : walls) {
			wall->render();
		}
		draw::restore_color();
	}
	void init() {
		for(const auto& t : TEXTURES) {
			std::string file = "../assets/apartment-assets/";
			if(t == -1) {
				file += "neg1.bmp";
			} else {
				file += std::to_string(t) + ".bmp";
			}
			textures::map_assets[t] = std::make_unique<Actor>(0,0,file.c_str());
		}
	}
};
