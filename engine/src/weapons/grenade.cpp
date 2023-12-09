#include <SDL2/SDL.h>
#include <iostream>
#include <array>

#include "grenade.hpp"
#include "../rng.hpp"
#include "../player.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[WEAPONS][GRENADE][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[WEAPONS][GRENADE][ERROR]: " << A << "\n";
namespace weapons{
  static constexpr size_t MAX_GRENADE = 12;
  static std::array<std::unique_ptr<Grenade>,MAX_GRENADE> ptr_memory;
  static std::vector<Grenade*> travelers;
  namespace grenade {
    void init(){
      m_debug("init");
      for(size_t i=0; i < MAX_GRENADE;i++){
        ptr_memory[i] = nullptr;
      }
      SDL_Point src{plr::cx(),plr::cy()};
      SDL_Point dst{plr::cx() + 850,plr::cy()};
      ptr_memory[0] = std::make_unique<Grenade>(src,dst);
      travelers.emplace_back(ptr_memory[0].get());
    }
    void tick(){
      for(size_t i=0; i < MAX_GRENADE;i++){
        if(ptr_memory[i] && !ptr_memory[i]->done()){
          ptr_memory[i]->tick();
        }
      }
    }
  };
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
  int Grenade::hold_grenade(){

    return 0;
  }
  int Grenade::toss_towards(const int32_t& dst_x, const int32_t& dst_y){

    return 0;
  }
  void Grenade::explode(){

  }
  void Grenade::tick(){
    SDL_Rect r;
    ++line_index;
    if(line_index >= line.points.size()){
      line_index = 0;
    }
    r.x = line.points[line_index].x;
    r.y = line.points[line_index].y;
    r.w = 10;
    r.h = 10;
    draw::blatant_rect(&r);
    draw::line(source.x,source.y,r.x,r.y);
  }
  bool Grenade::done(){
    return m_done;
  }
};
