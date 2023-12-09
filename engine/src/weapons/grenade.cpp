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
    
    void move_map(int dir, int amount){
      LOCK_MUTEX(travelers_mutex);
      for(auto& t: travelers){
        if(t->done()){
          continue;
        }
        switch(dir) {
          case NORTH_EAST:
            t->dest_rect.y += amount;
            t->dest_rect.x -= amount;
            break;
          case NORTH_WEST:
            t->dest_rect.y += amount;
            t->dest_rect.x += amount;
            break;
          case NORTH:
            t->dest_rect.y += amount;
            break;
          case SOUTH_EAST:
            t->dest_rect.y -= amount;
            t->dest_rect.x -= amount;
            break;
          case SOUTH_WEST:
            t->dest_rect.y -= amount;
            t->dest_rect.x += amount;
            break;
          case SOUTH:
            t->dest_rect.y -= amount;
            break;
          case WEST:
            t->dest_rect.x += amount;
            break;
          case EAST:
            t->dest_rect.x -= amount;
            break;
          default:
            break;
        }
        for(auto& exp : t->line.points){
          switch(dir) {
            case NORTH_EAST:
              exp.y += amount;
              exp.x -= amount;
              break;
            case NORTH_WEST:
              exp.y += amount;
              exp.x += amount;
              break;
            case NORTH:
              exp.y += amount;
              break;
            case SOUTH_EAST:
              exp.y -= amount;
              exp.x -= amount;
              break;
            case SOUTH_WEST:
              exp.y -= amount;
              exp.x += amount;
              break;
            case SOUTH:
              exp.y -= amount;
              break;
            case WEST:
              exp.x += amount;
              break;
            case EAST:
              exp.x -= amount;
              break;
            default:
              break;
          }
        }
      }
      UNLOCK_MUTEX(travelers_mutex);
    }

  };
  Grenade::Grenade(){
    m_done = true;
    source = {};
    dest = {};
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
    line_index = 0;
  }
  Grenade::Grenade(explosive_stats_t* in_stats){
    stats = in_stats;
  }
  void Grenade::set_grenade(explosive_stats_t* in_stats,const int32_t& src_x,const int32_t& src_y){
    stats = in_stats;
    source.x = src_x;
    source.y = src_y;
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
    line_index = 0;
    dest_rect.x = dst_x - 50;
    dest_rect.y = dst_y - 50;
    dest_rect.w = 80;
    dest_rect.h = 80;

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
    draw::line(source.x,source.y,r.x,r.y);
    draw::blatant_rect(&dest_rect);
    if(SDL_IntersectRect(
          &r,
          &dest_rect,
          &result)) {
      m_debug("DETONATE");
      SDL_Point p{dest_rect.x + 50,dest_rect.y + 50};
      damage::explosions::detonate_at(&p,//SDL_Point* p,
          rand_between(50,180),//const uint16_t& radius, 
          rand_between(180,360),//const uint16_t& damage,
          rand_between(0,3)                      //const uint8_t& type);
      );
      m_done = true;
    }
  }
  bool Grenade::done(){
    return m_done;
  }
};
