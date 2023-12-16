#include "bullet.hpp"
#include "player.hpp"
#include "tick.hpp"
#include "world.hpp"
#include "colors.hpp"
#include "font.hpp"
#include "npc-spetsnaz.hpp"
#include <SDL2/SDL.h>
#include "direction.hpp"
#include "draw-state/ammo.hpp"
#include "constants.hpp"
#include "damage/explosions.hpp"
#include <SDL2/SDL_image.h>
#include <map>

extern SDL_Window* win;
#define m_debug(A) std::cerr << "[DEBUG]:BULLET.CPP: " << A << "\n";
//#define DRAW_VECTOR_BULLET_TRAIL
namespace bullet {
  static std::unique_ptr<BulletPool> pool = nullptr;
  static Actor bullet_trail;
  static constexpr double PI = 3.14159265358979323846;
  static Actor mp5;
  static Actor p226;
  struct damage_display_t {
    SDL_Point where;
    std::pair<int,int> damage_amount;
    uint64_t display_until;
  };
  static std::vector<damage_display_t> damage_display_list;
  //Line line;
  int radius;
  BulletPool::BulletPool()  {
    for(std::size_t i=0; i < POOL_SIZE; ++i) {
      bullets[i] = nullptr;
    }
    index = 0;
  }
  Bullet::Bullet() {
    done = 0;
    line_index = 0;
    rect.w = 20;
    rect.h = 20;
    initialized = true;
  }
  void Bullet::clear() {
    done = true;
    initialized = false;
    trimmed.clear();
    line.points.clear();
  }
  void Bullet::calc() {
    clear();

    distance = closest = 9999;
    line_index = 0;

    angle = coord::get_angle(src.x,src.y,dst.x,dst.y);
    line.p1.x = src.x;
    line.p1.y = src.y;
    line.p2.x = (1000 * win_width()) * cos(PI * 2  * angle / 360);
    line.p2.y = (1000 * win_height()) * sin(PI * 2 * angle / 360);

    line.getPoints(INITIAL_POINTS);
    circle_points = shapes::CaptureDrawCircle(src.x,src.y, radius);
    for(const auto& cp : circle_points) {
      for(const auto& pt : line.points) {
        distance = sqrt(pow(pt.x - cp.x,2) + pow(pt.y - cp.y, 2) * 1.0);
        if(distance < closest) {
          line.p1.x = pt.x;
          line.p1.y = pt.y;
          closest = distance;
          if(distance < 60) {
            break;
          }
        }
      }
    }
    for(const auto& point : line.points) {
      if(point.x < viewport::min_x() || point.x > viewport::max_x() ||
          point.y < viewport::min_y() || point.y > viewport::max_y()) {
        dst.x = line.p2.x = point.x;
        dst.y = line.p2.y = point.y;
        break;
      }
    }
    line.getPoints(INITIAL_POINTS);
    dst.x = line.p2.x;
    dst.y = line.p2.y;
    rect.x = line.p1.x;
    rect.y = line.p1.y;
    current.x = line.p1.x;
    current.y = line.p1.y;
    for(const auto& _line_p : line.points) {
      distance = sqrt(pow(current.x - _line_p.x,2) + pow(current.y - _line_p.y, 2) * 1.0);
      if(distance >= (*stats)[WPN_PIXELS_PT]) {
        trimmed.emplace_back();
        auto& r = trimmed.back();
        r.x = _line_p.x;
        r.y = _line_p.y;
        current.x = _line_p.x;
        current.y = _line_p.y;
      }
    }
    current.x = line.p1.x;
    current.y = line.p1.y;
    line.points.clear();
    initialized = true;
  }
  bool Bullet::needs_processing() {
    return !done && initialized;
  }
  SDL_Texture* Bullet::bullet_trail_texture() {
    return bullet_trail.bmp[0].texture;
  }
  void Bullet::draw_bullet_trail() {
    auto dst = rect;
    int angle = coord::get_angle(src.x,src.y,rect.x,rect.y);
    angle += 90;
    dst.h = constants::BULLET_TRAIL_HEIGHT;//20;
    dst.w = constants::BULLET_TRAIL_WIDTH;//9;
    dst.y -= dst.h / 2;
#ifdef DRAW_SURROUNDING_BULLET_TRAIL_RECT
    draw::rect(&dst);
#endif
    SDL_RenderCopyEx(
        ren,  //renderer
        bullet_trail_texture(),
        nullptr,// src rect
        &dst, // dst rect
        angle, // angle
        nullptr,  // center
        SDL_FLIP_NONE // flip
        );
  }
  void Bullet::travel() {
    if(line_index >= trimmed.size() - 1) {
      clear();
      return;
    }
    rect.x = trimmed[line_index].x;
    rect.y = trimmed[line_index].y;
    SDL_Rect result;

    bool impact = 0;
    if(is_npc) {
      if(SDL_IntersectRect(
            &rect,
            plr::get_rect(),
            &result)) {
        /**
         * TODO: if npc's weapon deals explosive damage... then borrow code from below
         */
        plr::take_damage(stats);
        impact = 1;
      }
    } else {
      for(auto& npc : world->npcs) {
        if(SDL_IntersectRect(
              &rect,
              &npc->rect,
              &result)) {
          /*
           * TODO: if player's weapon deals explosive damage:
           * if(plr::deals_explosive_damage()){
              SDL_Point p{rect.x,rect.y};
              damage::explosions::detonate_at(&p,
                radius,
                damage,
                type);
           }
          */
          if(npc::is_dead(npc)){
            continue;
          }
          auto p = plr::gun_damage();
          damage_display_list.emplace_back(SDL_Point{npc->rect.x,npc->rect.y},p,tick::get() + 2500);
          npc::take_damage(npc,p.first + p.second);
          impact = 1;
        }
      }
      if(!impact){
        // TODO: optimize this
        for(auto& w : wall::blockable_walls){
          if(SDL_IntersectRect(
                &rect,
                &w->rect,
                &result)){
            impact = 1;
            break;
          }
        }
      }
    }
    if(impact) {
#ifdef TEST_PLAYER_EXPLOSIONS
      if(!is_npc){
        SDL_Point p{rect.x,rect.y};
        SDL_Rect s{src.x,src.y};
        damage::explosions::detonate_from(&p,                     //SDL_Point* p,
            rand_between(50,120),   //const uint16_t& radius, 
            rand_between(150,350),  //const uint16_t& damage,
            rand_between(0,3),      //const uint8_t& type,
            s
        );
      }
#endif
      clear();
      return;
    }
    draw_bullet_trail();
    current.x = rect.x;
    current.y = rect.y;
    ++line_index;
  }

  void Bullet::report() {
#ifdef BULLET_REPORT
    std::cout << "Bullet: " << src.x << "x" << src.y <<
      "=>" << dst.x << "x" << dst.y << "\n";
#endif
  }
  void BulletPool::queue(weapon_stats_t* stats_ptr) {
    if(index >= POOL_SIZE) {
      index = 0;
    }

    if(bullets[index] == nullptr){
      bullets[index] = std::make_unique<Bullet>();
    }
    bullets[index]->stats = stats_ptr;
    bullets[index]->src.x = plr::get_cx();
    bullets[index]->src.y = plr::get_cy();
    bullets[index]->dst.x = cursor::mx();
    bullets[index]->dst.y = cursor::my();
    bullets[index]->is_npc = false;
    bullets[index]->calc();
    bullets[index]->done = false;
    bullets[index]->initialized = true;
    ++index;
  }
  void BulletPool::queue_npc(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx, int in_cy,int dest_x,int dest_y) {
    if(index >= POOL_SIZE) {
      index = 0;
    }

    if(bullets[index] == nullptr){
      bullets[index] = std::make_unique<Bullet>();
    }
    bullets[index]->npc_id = in_npc_id;
    bullets[index]->is_npc = true;
    bullets[index]->stats = stats_ptr;
    bullets[index]->src.x = in_cx;
    bullets[index]->src.y = in_cy;
    bullets[index]->dst.x = dest_x;
    bullets[index]->dst.y = dest_y;
    bullets[index]->calc();
    bullets[index]->done = false;
    bullets[index]->initialized = true;
    ++index;
  }
  void queue_bullets(weapon_stats_t* stats_ptr) {
    if(!pool){
      m_debug("queue_bullets stats_ptr encounted a null pool!");
      pool = std::make_unique<BulletPool>();
    }
    pool->queue(stats_ptr);
  }
  void queue_npc_bullets(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx,int in_cy,int dest_x, int dest_y) {
    if(!pool){
      m_debug("queue_npc_bullets encounted a null pool!");
      pool = std::make_unique<BulletPool>();
    }
    pool->queue_npc(in_npc_id,stats_ptr,in_cx,in_cy,dest_x,dest_y);
  }
  void draw_ammo() {
    if(!draw_state::ammo::draw_ammo()) {
      return;
    }
    static SDL_Point where{0,0};
    static uint16_t height = 25;
    static uint16_t width = 300;
    std::string msg = plr::get()->equipped_weapon_name + " (";
    msg += std::to_string(plr::ammo()) + "/";
    msg += std::to_string(plr::total_ammo()) + ")";
    if(*plr::get()->ammo == 0) {
      font::red_text(&where,msg,height,width);
    } else {
      font::green_text(&where,msg,height,width);
    }
  }
#ifdef DRAW_WEAPON
  void draw_weapon(){
    int angle = 0;
    SDL_RenderCopyEx(
        ren,  //renderer
        mp5.bmp[0].texture,
        nullptr,// src rect
        plr::get_rect(), // dst rect
        angle, // angle
        nullptr,  // center
        SDL_FLIP_NONE // flip
        );
  }
#endif
  void tick() {
    for(auto& damage_display : damage_display_list){
      damage_display.where.y -= 8;
          auto pair = damage_display.damage_amount;
          auto display = damage_display.where;
	        font::red_text(&display, //const SDL_Point* where,
                         std::to_string(pair.first),    //const std::string& msg,
                         20,//const uint16_t& height,
                         50//const uint16_t& width);
          );
          if(pair.second > 0){
            display.y = damage_display.where.y + 50;
            font::green_text(&display, //const SDL_Point* where,
                           std::to_string(pair.second),    //const std::string& msg,
                           40,//const uint16_t& height,
                           80//const uint16_t& width);
            );
          }
    }
    std::erase_if(damage_display_list,[](const auto& d){ return d.display_until <= tick::get(); });
    draw_ammo();
#ifdef DRAW_WEAPON
    draw_weapon();
#endif
    for(size_t i=0; i < BulletPool::POOL_SIZE;i++){
      if(pool->bullets[i] == nullptr){
        continue;
      }
      if(pool->bullets[i]->needs_processing()){
        pool->bullets[i]->travel();
      }
      if(pool->bullets[i]->done) {
        continue;
      }
      pool->bullets[i]->initialized = true;
      SDL_Rect source;
      source.x = pool->bullets[i]->src.x;
      source.y = pool->bullets[i]->src.y;
      source.w = 10;
      source.h = 10;
      SDL_Rect dest;
      dest.x = pool->bullets[i]->dst.x;
      dest.y = pool->bullets[i]->dst.y;
      dest.w = 10;
      dest.h = 10;
      auto angle = coord::get_angle(source.x,source.y,dest.x,dest.y);
      dest.x = (1000 * win_width()) * cos(PI * 2  * angle / 360);
      dest.y = (1000 * win_height()) * sin(PI * 2 * angle / 360);
#ifdef DRAW_BULLET_LINE
      draw::bullet_line(
          source.x,
          source.y,
          dest.x,
          dest.y 
          );
#endif
      if(pool->bullets[i]->is_npc) {
        SDL_Rect result;
        result.x = 0;
        result.y = 0;
        result.w = 0;
        result.y = 0;
        if(SDL_IntersectRect(
              &dest,
              plr::get_rect(),
              &result)) {
          plr::take_damage(pool->bullets[i]->stats);
        }
        continue;
      }
      for(const auto& npc : world->npcs) {
        SDL_Rect result;
        result.x = 0;
        result.y = 0;
        result.w = 0;
        result.y = 0;
        /*
           std::cout << "source: " << &source << "\n";
           std::cout << "npc: " << npc << "\n";
           std::cout << "npc->rect: " << npc->rect.x << "," << npc->rect.y << "\n";
           std::cout << "result: " << &result << "\n";
           */
        if(SDL_IntersectRect(
              &source,
              &npc->rect,
              &result)) {
          auto p = plr::gun_damage();
          npc::take_damage(npc,p.first + p.second);
        }
      }
    }
  }
  void init() {
    bullet_trail.x = 0;
    bullet_trail.y = 0;
    bullet_trail.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
    mp5.load_bmp_asset("../assets/mp5.bmp");
    p226.load_bmp_asset("../assets/p226.bmp");
    radius = 55;
    pool = std::make_unique<BulletPool>();
  }
  void cleanup_pool() {
  }
  void program_exit(){
  }
};
#undef m_debug
