#include <SDL2/SDL.h>
#include <iostream>
#include <array>

#include "grenade.hpp"
#include "../rng.hpp"
#include "../player.hpp"
#include "../damage/explosions.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[WEAPONS][GRENADE][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[WEAPONS][GRENADE][ERROR]: " << A << "\n";
namespace weapons{
  static constexpr size_t MAX_GRENADE = 12;
  //static std::array<std::unique_ptr<Grenade>,MAX_GRENADE> ptr_memory;
  static std::vector<Grenade*> travelers;
  static SDL_mutex* travelers_mutex = SDL_CreateMutex();
  namespace grenade {
    void init(){
      m_debug("init");
    }
    void tick(){
      for(auto& traveler : travelers){
        if(!traveler->done()){
          traveler->tick();
        }
      }
    }
    void register_traveler(Grenade* ptr){
      LOCK_MUTEX(travelers_mutex);
      travelers.emplace_back(ptr);
      UNLOCK_MUTEX(travelers_mutex);
      m_debug("travelers: " << travelers.size());
    }
  };
  Grenade::Grenade(){
    m_done = true;
    source = {};
    dest = {};
    register_actor(&self);
  }
  Grenade::Grenade(const SDL_Point& src,const SDL_Point& dst){
    source = src;
    dest = dst;
    m_done = false;
    auto angle = coord::get_angle(source.x,source.y,dest.x,dest.y);
    line.p1.x = source.x;
    line.p1.y = source.y;
    line.p2.x = dest.x;//(win_width()) * cos(PI * 2  * angle / 360);
    line.p2.y = dest.y;//(win_height()) * sin(PI * 2 * angle / 360);

    line.getPoints(1024);
    line.register_with_movement_system();
    line_index = 0;
    self.rect.x = dst.x;
    self.rect.y = dst.y;
    self.rect.w = 80;
    self.rect.h = 80;
    register_actor(&self);
  }
  Grenade::Grenade(explosive_stats_t* in_stats){
    stats = in_stats;
    //self.rect.x = 0;
    //self.rect.y = 0;
    //self.rect.w = 80;
    //self.rect.h = 80;
    register_actor(&self);
  }
  void Grenade::set_grenade(explosive_stats_t* in_stats,const int32_t& src_x,const int32_t& src_y){
    stats = in_stats;
    source.x = src_x;
    source.y = src_y;
    //self.rect.x = src_x;
    //self.rect.y = src_y;
    //self.rect.w = 80;
    //self.rect.h = 80;
  }
  int Grenade::hold_grenade(){
    return 0;
  }
  int Grenade::toss_towards(const int32_t& dst_x, const int32_t& dst_y){
    m_done = false;
    dest.x = dst_x;
    dest.y = dst_y;
    auto angle = coord::get_angle(source.x,source.y,dest.x,dest.y);
    line.p1.x = source.x;
    line.p1.y = source.y;
    line.p2.x = dest.x;
    line.p2.y = dest.y;

    line.getPoints(30);
    line.register_with_movement_system();
    line_index = 0;
    self.rect.x = dst_x - 50;
    self.rect.y = dst_y - 50;
    self.rect.w = 80;
    self.rect.h = 80;

    return 0;
  }
  void Grenade::explode(){

  }
  void Grenade::tick(){
    SDL_Rect r;
    ++line_index;
    if(line_index >= line.points.size()){
      m_done = true;
      return;
    }
    r.x = line.points[line_index].x;
    r.y = line.points[line_index].y;
    r.w = 10;
    r.h = 10;
    draw::blatant_rect(&r);
#ifdef DRAW_GRENADE_DEBUG
    draw::line(source.x,source.y,r.x,r.y);
    draw::blatant_rect(&self.rect);
#endif
    if(SDL_IntersectRect(
          &r,
          &self.rect,
          &result)) {
      m_debug("DETONATE");
      SDL_Point p{self.rect.x + 50,self.rect.y + 50};
      damage::explosions::detonate_at(&p,//SDL_Point* p,
          rand_between(50,180),//const uint16_t& radius, 
          rand_between(180,360),//const uint16_t& damage,
          rand_between(0,3)                      //const uint8_t& type);
      );
      line.unregister_with_movement_system();
      m_done = true;
    }
  }
  bool Grenade::done(){
    return m_done;
  }
};
