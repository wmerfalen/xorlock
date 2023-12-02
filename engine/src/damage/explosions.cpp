#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "explosions.hpp"
#include "../player.hpp"
#include "../direction.hpp"
#include "../npc/paths.hpp"
#include "../font.hpp"
#include "../colors.hpp"
#include "../rng.hpp"
#include "../sound/gunshot.hpp"
#include "../sound/npc.hpp"
#include "../viewport.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[DAMAGE][explosion][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[DAMAGE][explosion][ERROR]: " << A << "\n";

namespace damage::explosions {
  static constexpr std::size_t explosion_WIDTH = 375;
  static constexpr std::size_t explosion_HEIGHT = 260;
  static int mode = 0;
  static constexpr std::size_t MAX_EXPLOSIONS_LIST_SIZE = 1024;
  static std::vector<explosion*> active_explosion_pointers;
  static std::array<std::unique_ptr<explosion>,MAX_EXPLOSIONS_LIST_SIZE> ptr_memory_pool;
  static std::size_t EXPLOSION_DIR_START = 0;
  static std::size_t EXPLOSION_DIR_STOP = 3;
  void move_map(int dir, int amount){
    for(auto& exp : active_explosion_pointers){
      switch(dir) {
        case NORTH_EAST:
          exp->self.rect.y += amount;
          exp->self.rect.x -= amount;
          break;
        case NORTH_WEST:
          exp->self.rect.y += amount;
          exp->self.rect.x += amount;
          break;
        case NORTH:
          exp->self.rect.y += amount;
          break;
        case SOUTH_EAST:
          exp->self.rect.y -= amount;
          exp->self.rect.x -= amount;
          break;
        case SOUTH_WEST:
          exp->self.rect.y -= amount;
          exp->self.rect.x += amount;
          break;
        case SOUTH:
          exp->self.rect.y -= amount;
          break;
        case WEST:
          exp->self.rect.x += amount;
          break;
        case EAST:
          exp->self.rect.x -= amount;
          break;
        default:
          break;
      }
    }
  }
  void draw_target(SDL_Point p){
    SDL_Rect r;
    r.x = p.x;
    r.y = p.y;
    r.w = 50;
    r.h = 50;
    draw::blatant_rect(&r);
  }

  int initial_x = 0;
  int initial_y = 0;
  std::vector<std::pair<int,int>> target_offsets = {
    {0,0},
    {125,-125},
    {400,-125},
    {125,125},
    {400,125},
  };
  static inline void cast_rays(){
    const int& x = plr::cx();
    const int& y = plr::cy();
    for(const auto& p : target_offsets){
      draw_target({x + p.first, y + p.second});
    }
  }
  static inline std::vector<SDL_Point> save_bomb_targets(){
    const int& x = plr::cx();
    const int& y = plr::cy();
    std::vector<SDL_Point> list;
    for(const auto& p : target_offsets){
      list.emplace_back(x + p.first,y + p.second);
    }
    return list;
  }
  void init() {
    m_debug("init entry");
    for(size_t i=0; i < MAX_EXPLOSIONS_LIST_SIZE;i++){
      ptr_memory_pool[i] = nullptr;
    }
  }
  uint64_t next_space_bar_accepted_at = 0;
  void space_bar_pressed(){
    if(next_space_bar_accepted_at > tick::get()){
      return;
    }
    next_space_bar_accepted_at = tick::get() + 60000;
    //bomb_targets = save_bomb_targets();
  }
  void explosion::move_to(const int32_t& x,const int32_t& y) {
    self.rect.x = x;
    self.rect.y = y;
  }
  void tick() {
    static bool initial_set = false;
    if(!initial_set){
      initial_x = plr::cx();
      initial_y = plr::cy();
      initial_set = true;
    }

    for(auto& ptr : active_explosion_pointers){
      if(ptr->done){
        continue;
      }
      ptr->tick();
      SDL_RenderCopyEx(
          ren,  //renderer
          ptr->self.bmp[ptr->phase % ptr->self.bmp.size()].texture,
          nullptr,// src rect
          &ptr->self.rect,
          ptr->angle, // angle
          nullptr,  // center
          SDL_FLIP_NONE // flip
          );
    }
  }
  SDL_Texture* explosion::initial_texture() {
    return self.bmp[0].texture;
  }
  explosion::explosion(uint8_t directory_id) : angle(0),
  explosive_damage(rand_between(500,800)),
  radius(rand_between(100,450)),
  x(0),y(0),done(false) {
    std::string path = "";
    for(size_t i=0; i < strlen(top_level_dir_pattern);i++){
      if(0 == strncmp(&top_level_dir_pattern[i],top_level_dp_replace,strlen(top_level_dp_replace))){
        path += std::to_string(directory_id);
        i += strlen(top_level_dp_replace) - 1;
        continue;
      }
      path += top_level_dir_pattern[i];
    }
    m_debug("path: '" << path << "'");
    self.rect.x = x;
    self.rect.y = y;
    self.rect.w = radius * 2;
    self.rect.h = radius * 2;
    self.load_bmp_assets(path.c_str(),4,1);
    start_tick = tick::get();

    move_to(x,y);
  }
  void explosion::tick() {
    if(start_tick + 1500 < tick::get()){
      ++phase;
      start_tick = tick::get();
    }
    if(phase >= self.bmp.size()){
      done = true;
    }
  }
  Asset* explosion::next_state() {
    return states[0];
  }

  void detonate_at(SDL_Point* p,int damage,int type){
    m_debug("DETONATE_AT: " << p->x << "," << p->y << " [" << damage << "](" << type << ")");

    for(size_t i=0; i < MAX_EXPLOSIONS_LIST_SIZE; i++){
      if(!ptr_memory_pool[i]){
        m_debug("found empty ptr: " << i);
        ptr_memory_pool[i] = std::make_unique<explosion>(type);
        ptr_memory_pool[i]->move_to(p->x,p->y);
        ptr_memory_pool[i]->done = false;
        ptr_memory_pool[i]->trigger_explosion();
        active_explosion_pointers.emplace_back(ptr_memory_pool[i].get());
        return;
      }
      if(ptr_memory_pool[i]->done){
        m_debug("found ->done: " << i);
        ptr_memory_pool[i]->move_to(p->x,p->y);
        ptr_memory_pool[i]->done = false;
        ptr_memory_pool[i]->trigger_explosion();
        active_explosion_pointers.emplace_back(ptr_memory_pool[i].get());
        return;
      }
    }
  }

  void explosion::trigger_explosion(){
    m_debug("trigger_explosion()");
    done = false;
    phase = 0;
    start_tick = tick::get();
    SDL_RenderCopyEx(
        ren,  //renderer
        self.bmp[phase].texture,
        nullptr,// src rect
        &self.rect,
        angle, // angle
        nullptr,  // center
        SDL_FLIP_NONE // flip
    );
  }
};

#undef m_debug
