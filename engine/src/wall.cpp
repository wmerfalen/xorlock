#include "direction.hpp"
#include "wall.hpp"
#include "map.hpp"
#include "world.hpp"
#include "player.hpp"
#include <functional>

// FIXME
#define SHOW_HELPFUL_GRAPH_STUFF
#ifdef SHOW_HELPFUL_GRAPH_STUFF
#define DRAW_GATEWAYS 1
#endif

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[WALL][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[WALL][ERROR]: " << A << "\n";
namespace npc::paths {
extern void load_los_cache();
};
std::vector<size_t> rendered;
size_t player_on_tile;
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
  std::vector<wall::Wall*> spawn_tiles;
  std::vector<wall::Wall*> npc_waypoints;
  std::string to_string(Texture t) {
    if(t==NPC_WAYPOINT_HELPER){
      return "NPC_WAYPOINT_HELPER";
    }
    if(t==PORTAL){
      return "PORTAL";
    }
    if(t==SPAWN_TILE){
      return "SPAWN_TILE";
    }
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
  rect{_x,_y,_width,_height}, ignore(true) {
    initialized = true;
    orig_rect = {_x,_y,_width,_height};
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
    auto ptr = std::make_unique<Wall>(_x,_y,_width,_height,_type);
    walls.emplace_back(std::move(ptr));
    auto raw_ptr = walls.back().get();
    if(raw_ptr->walkable) {
      walkable_walls.emplace_back(raw_ptr);
    } else {
      blockable_walls.emplace_back(raw_ptr);
      blocked.insert(raw_ptr);
    }
    if(raw_ptr->type == SPAWN_TILE){
      spawn_tiles.emplace_back(raw_ptr);
    }
  }
  bool Wall::build_check(){
    actor_ptr = textures::map_assets[type].get();
    texture = nullptr;
    if(!actor_ptr){
      ignore = true;
      return false;
    }
    if(actor_ptr->bmp.size() == 0){
      ignore = true;
      return false;
    }
    if(actor_ptr->bmp[0].texture == nullptr){
      ignore = true;
      return false;
    }
    texture = actor_ptr->bmp[0].texture;
    ignore = false;
    return true;
  }
  void Wall::render() {
#ifdef NO_WALKABLE_TEXTURES
    if(walkable) {
      return;
    }
#endif

#ifdef NO_WALL_TEXTURES
#else
    SDL_RenderCopy(ren, texture, nullptr, &rect);
#endif
  }
  void tick() {
    rendered.clear();
    std::size_t ctr=0;
    auto tile = npc::paths::get_tile(plr::self());
    if(tile){
      const auto wall_size = wall::walls.size();
#ifdef USE_DEFAULT_1024_WINDOW_WIDTH
      // TODO: "units" should be 1.0
      // This runs perfectly when WIN_WIDTH = 1024
      for(int multiplier=6; multiplier > -7; multiplier--){
        if(tile->index - (64 * multiplier) - 8 >= 0){
          for(int i=tile->index - (64 * multiplier) - 8; i < tile->index - (64 * multiplier) + 7 && i < wall_size;i++){
            wall::walls[i]->render();
          }
        }
      }
#else
      // TODO: "units" should be 1.5
      // TODO: dynamically calculate this
      // This runs perfectly when WIN_WIDTH = 1024 * 1.5
      for(int multiplier=6; multiplier > -8; multiplier--){
        if(tile->index - (64 * multiplier) - 8 >= 0){
          for(int i=tile->index - (64 * multiplier) - 8; i < tile->index - (64 * multiplier) + 9 && i < wall_size;i++){
            wall::walls[i]->render();
          }
        }
      }
#endif
    }
    //for(auto& wall : walls) {
    //  if((wall->rect.x >= plr::cx() - (win_width() / 2 + 300) && wall->rect.x <= plr::cx() + (win_width() / 2)) && 
    //      (wall->rect.y >= plr::cy() - (win_height() / 2 + 100) && wall->rect.y <= plr::cy() + (win_height() / 2))){
    //    wall->render();
    //    rendered.emplace_back(ctr);
    //  }
    //  ++ctr;
    //}
  }
  void draw_gateways(){
#ifdef DRAW_GATEWAYS
    for(const auto& wall : walls){
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
    }
#endif
  }
  wall::Wall* start_tile() {
    return start_tile_ptr;
  }
  void init() {
    m_debug("wall::init()");
    start_tile_ptr = nullptr;
    for(const auto& t : TEXTURES) {
      std::string file = "../assets/apartment-assets/";
      if(t == -1) {
        file += "neg1.bmp";
      } else {
        file += std::to_string(t) + ".bmp";
      }
      textures::map_assets[t] = std::make_unique<Actor>(0,0,file.c_str());
    }
    for(auto& w : walls) {
      w->build_check();
    }
    std::cout << "walkable_walls before cleanup: " << walkable_walls.size() << "\n";
    std::erase_if(walkable_walls,[&](auto& ptr) {
        return ptr->ignore;
        });
    std::cout << "blockable_walls before cleanup: " << blockable_walls.size() << "\n";
    std::erase_if(blockable_walls,[&](auto& ptr) {
        return ptr->ignore;
        });
    std::cout << "blocked before cleanup: " << blocked.size() << "\n";
    std::erase_if(blocked,[&](auto& ptr) {
        return ptr->ignore;
        });
    std::cout << "walls before cleanup: " << walls.size() << "\n";
    std::erase_if(walls,[&](auto& ptr) {
        return ptr->ignore;
        });
    for(const auto& w : walls){
      if(w->type == START_TILE) {
        start_tile_ptr = w.get();
        break;
      }
    }
    std::cout << "walkable_walls AFTER cleanup: " << walkable_walls.size() << "\n";
    std::cout << "blockable_walls AFTER cleanup: " << blockable_walls.size() << "\n";
    std::cout << "blocked AFTER cleanup: " << blocked.size() << "\n";
    std::cout << "walls AFTER cleanup: " << walls.size() << "\n";
    for(size_t i=0; i < walls.size();i++){
      walls[i]->index = i;
    }
    for(const auto& w : walls){
      if(w->type == NPC_WAYPOINT_HELPER){
        npc_waypoints.emplace_back(w.get());
      }
    }
    npc::paths::load_los_cache();
    m_debug("found " << npc_waypoints.size() << " NPC_WAYPOINT_HELPER TILES");
  }
  //std::vector<Wall*> gateways;
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
  bool is_blocked(SDL_Rect* r){
    SDL_Rect result;
    for(const auto& w : blockable_walls){
      if(SDL_IntersectRect(r,&w->rect,&result)){
        return true;
      }
    }
    return false;
  }
};
