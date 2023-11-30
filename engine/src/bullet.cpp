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

#define m_debug(A) std::cerr << "[DEBUG]:BULLET.CPP: " << A << "\n";
//#define DRAW_VECTOR_BULLET_TRAIL
namespace bullet {
  static std::unique_ptr<BulletPool> pool;
  static Actor bullet_trail;
  static constexpr double PI = 3.14159265358979323846;
  //Line line;
  int radius;
  BulletPool::BulletPool()  {
    for(std::size_t i=0; i < POOL_SIZE; ++i) {
      bullets[i] = std::make_unique<Bullet>();
      bullets[i]->clear();
    }
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
        plr::take_damage(stats);
        impact = 1;
      }
    } else {
      for(auto& npc : world->npcs) {
        if(SDL_IntersectRect(
              &rect,
              &npc->rect,
              &result)) {
          npc::take_damage(npc,plr::gun_damage());
          impact = 1;
        }
      }
    }
    if(impact) {
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
    if(index >= POOL_SIZE -1) {
      index = 0;
    }

    auto& r = this->bullets[index];
    r->stats = stats_ptr;
    r->src.x = plr::get_cx();
    r->src.y = plr::get_cy();
    r->dst.x = cursor::mx();
    r->dst.y = cursor::my();
    r->is_npc = false;
    r->calc();
    r->done = false;
    r->initialized = true;
    ++index;
  }
  void BulletPool::queue_npc(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx, int in_cy,int dest_x,int dest_y) {
    if(index >= POOL_SIZE -1) {
      index = 0;
    }

    auto& r = this->bullets[index];
    r->npc_id = in_npc_id;
    r->is_npc = true;
    r->stats = stats_ptr;
    r->src.x = in_cx;
    r->src.y = in_cy;
    r->dst.x = dest_x;
    r->dst.y = dest_y;
    r->calc();
    r->done = false;
    r->initialized = true;
    ++index;
  }
  void queue_bullets(weapon_stats_t* stats_ptr) {
    pool->queue(stats_ptr);
  }
  void queue_npc_bullets(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx,int in_cy,int dest_x, int dest_y) {
    pool->queue_npc(in_npc_id,stats_ptr,in_cx,in_cy,dest_x,dest_y);
  }
  void draw_ammo() {
    if(!draw_state::ammo::draw_ammo()) {
      return;
    }
    if(plr::get()->primary_equipped == false) {
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
  void tick() {
    static uint64_t count = 0;
    draw_ammo();
    count = 0;
    for(auto& bullet : pool->bullets) {
      if(bullet->needs_processing()){
        bullet->travel();
      }
      if(bullet->done) {
        continue;
      }
      //bullet->done = true;
      bullet->initialized = true;
      SDL_Rect source,dest,result;
      source.x = bullet->src.x;
      source.y = bullet->src.y;
      dest.x = bullet->dst.x;
      dest.y = bullet->dst.y;
      auto angle = coord::get_angle(source.x,source.y,dest.x,dest.y);
      dest.x = (1000 * win_width()) * cos(PI * 2  * angle / 360);
      dest.y = (1000 * win_height()) * sin(PI * 2 * angle / 360);
      //bullet->travel();
#ifdef DRAW_BULLET_LINE
      draw::bullet_line(
          source.x,
          source.y,
          dest.x,
          dest.y 
        );
#endif
      if(bullet->is_npc) {
        if(SDL_IntersectRect(
              &dest,
              plr::get_rect(),
              &result)) {
          plr::take_damage(bullet->stats);
        }
        continue;
      }
      for(auto& npc : world->npcs) {
        if(SDL_IntersectRect(
              &source,
              &npc->rect,
              &result)) {
          npc::take_damage(npc,plr::gun_damage());
        }
      }
    }
    }
    void init() {
      bullet_trail.x = 0;
      bullet_trail.y = 0;
      bullet_trail.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
      radius = 55;
      pool = std::make_unique<BulletPool>();

      // load WAV file


    }
    void cleanup_pool() {
    }
  };
#undef m_debug
