#include "direction.hpp"
#include "wall.hpp"
#include "map.hpp"
#include "world.hpp"
#include "player.hpp"
#include <functional>

#ifdef SHOW_HELPFUL_GRAPH_STUFF
#define DRAW_GATEWAYS 1
#endif

namespace wall {
  std::set<wall::Wall*> blocked;
  static wall::Wall* start_tile_ptr;
  namespace textures {
    static std::map<Texture,std::unique_ptr<Actor>> map_assets;
  };
  namespace calc {
    auto distance(int32_t x1, int32_t y1, int32_t x2,int32_t y2) {
      auto dx{x1 - x2};
      auto dy{y1 - y2};
      return std::sqrt(dx*dx + dy*dy);
    }
  };

  std::vector<std::unique_ptr<Wall>> walls;
  std::vector<Wall*> blockable_walls;
  std::vector<Wall*> walkable_walls;
  std::vector<wall::Wall*> gateways;
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
    if(t==START_TILE) {
      return "START_TILE";
    }

    return "<unknown>";
  }
  SDL_Rect collision;
  int32_t Wall::distance_to(wall::Wall* other) {
    return calc::distance(rect.x,rect.y,other->rect.x,other->rect.y);
  }
  int32_t Wall::distance_to(SDL_Rect* other) {
    return calc::distance(rect.x,rect.y,other->x,other->y);
  }
  Wall::Wall(
      const int& _x,
      const int& _y,
      const int& _width,
      const int& _height,
      Texture _type) : is_gateway(false), draw_color(nullptr),connections(0),why(0), type(_type),
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
    if(walls.back()->walkable) {
      walkable_walls.emplace_back(walls.back().get());
    } else {
      blockable_walls.emplace_back(walls.back().get());
      blocked.insert(walls.back().get());
    }
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
    //#define DEBUG_DONT_RENDER_WALL_TEXTURES
#ifndef DEBUG_DONT_RENDER_WALL_TEXTURES
    SDL_RenderCopy(ren, ptr->bmp[0].texture, nullptr, &rect);

#endif
#endif
  }
  void tick() {
    //draw::draw_green();
    for(auto& wall : walls) {
      wall->render();
#ifdef DRAW_GATEWAYS
      if(wall->is_gateway) {
        auto r = wall->rect;
        r.x += CELL_WIDTH / 2;
        r.y += CELL_HEIGHT / 2;
        draw::green_letter_at(&r,"g",50);
      }

      if(wall->connections && wall->connections < 4) {
        draw::grey_letter_at(&wall->rect,std::to_string(wall->connections),30);
      } else if(wall->connections >= 4) {
        if(wall->connections >= 8) {
          draw::green_letter_at(&wall->rect,std::to_string(wall->connections),40);
        } else {
          draw::green_letter_at(&wall->rect,std::to_string(wall->connections),30);
        }
      }
#endif
    }
    //draw::restore_color();
  }
  wall::Wall* start_tile() {
    return start_tile_ptr;
  }
  void init() {
    std::cout << "wall::init()\n";
    start_tile_ptr = nullptr;
    for(const auto& w : walls) {
      if(w->type == START_TILE) {
        start_tile_ptr = w.get();
        break;
      }
    }
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
  //std::vector<Wall*> gateways;
  bool is_blocked(wall::Wall * ptr) {
    return blocked.find(ptr) != blocked.cend();
  }
  void program_exit(){
    //static std::map<Texture,std::unique_ptr<Actor>> map_assets;
    for(auto& p : textures::map_assets){
      p.second = nullptr;
    }
    textures::map_assets.clear();
    //std::vector<std::unique_ptr<Wall>> walls;
    for(size_t i=0; i < walls.size();i++){
      walls[i] = nullptr;
    }
    walls.clear();
    blockable_walls.clear();
    walkable_walls.clear();
    gateways.clear();
    blocked.clear();
    start_tile_ptr = nullptr;
  }
};
