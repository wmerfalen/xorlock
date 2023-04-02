#include "extern.hpp"
#include "map.hpp"
#include "world.hpp"
#include "player.hpp"

namespace map {
	std::vector<std::unique_ptr<Wall>> walls;
	SDL_Rect collision;
	Wall::Wall(
	    const int& _x,
	    const int& _y,
	    const int& _width,
	    const int& _height) {
		rect = {_x,_y,_width,_height};
		initialized = true;
		std::cout << "rect.x: " << rect.x << "\n";
		std::cout << "rect.y: " << rect.y << "\n";
		std::cout << "rect.w: " << rect.w << "\n";
		std::cout << "rect.h: " << rect.h << "\n";

	}
	void Wall::render() {
		SDL_RenderDrawRect(ren,&rect);
	}
	bool can_move(int direction,int amount) {
		SDL_Rect result, p;
		for(const auto& wall : walls) {
			p = *plr::get_rect();
			p.w += amount;
			p.h += amount * 2;
			p.y -= amount;
#ifdef DRAW_COLLISIONS
			draw::draw_green();
			SDL_RenderDrawRect(ren,&p);
			draw::restore_color();
#endif
			bool can_before_adjustment = !SDL_IntersectRect(
			                                 &wall->rect,
			                                 &p,
			                                 &result);
#ifdef DRAW_COLLISIONS
			if(!can_before_adjustment) {
				std::cout << "result.x: " << result.x << "\n";
				std::cout << "result.y: " << result.y << "\n";
				std::cout << "result.w: " << result.w << "\n";
				std::cout << "result.h: " << result.h << "\n";
				draw::draw_red();
				SDL_RenderDrawRect(ren,&result);
				draw::restore_color();
			}
#endif
			switch(direction) {
				case NORTH:
					p.y -= amount;
					break;
				case SOUTH:
					p.y += amount;
					break;
				case WEST:
					p.x -= amount;
					break;
				case EAST:
					p.x += amount;
					break;
				default:
					break;
			}
			bool can_after_adjustment = !SDL_IntersectRect(
			                                &wall->rect,
			                                &p,
			                                &result);
			if(!can_before_adjustment && !can_after_adjustment) {
				return false;
			}
		}
		return true;
	}
	void move_map(int direction,int amount) {
		for(auto& wall : walls) {
			switch(direction) {
				case NORTH:
					wall->rect.y += amount;
					break;
				case SOUTH:
					wall->rect.y -= amount;
					break;
				case WEST:
					wall->rect.x += amount;
					break;
				case EAST:
					wall->rect.x -= amount;
					break;
				default:
					break;
			}
		}
	}
	void tick() {
		draw::draw_green();
		for(auto& wall : walls) {
			wall->render();
		}
		draw::restore_color();
	}
	void init() {
		/**
		 * Draw a tunnel that goes from the south to the north
		 */
		walls.emplace_back(std::make_unique<Wall>(200,500,100,900));
		walls.emplace_back(std::make_unique<Wall>(600,500,100,900));
	}
};
