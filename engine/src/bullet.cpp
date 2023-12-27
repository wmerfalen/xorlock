#include "bullet.hpp"
#include "player.hpp"
#include "tick.hpp"
#include "world.hpp"
#include "colors.hpp"
#include "font.hpp"
#include "npc-spetsnaz.hpp"
#include "npc-bomber.hpp"
#include <SDL2/SDL.h>
#include "direction.hpp"
#include "draw-state/ammo.hpp"
#include "constants.hpp"
#include "damage/explosions.hpp"
#include "abilities/turret.hpp"
#include <SDL2/SDL_image.h>
#include <map>

#include <cmath>  // Include the cmath header for the arctangent function

double slopeToAngle(double slope) {
    // Use the arctangent function (atan) to convert slope to angle
    double angleInRadians = atan(slope);

    // Convert radians to degrees
    double angleInDegrees = angleInRadians * 180.0 / M_PI;

    return angleInDegrees;
}
double angleBetweenPoints(double x1, double y1, double x2, double y2) {
    // Calculate the differences in x and y coordinates
    double deltaX = x2 - x1;
    double deltaY = y2 - y1;

    // Use the arctangent function (atan2) to calculate the angle
    double angleInRadians = atan2(deltaY, deltaX);

    // Convert radians to degrees
    double angleInDegrees = angleInRadians * 180.0 / M_PI;

    return angleInDegrees;
}
void calculateDestination(double x1, double y1, double angleDegrees, double distance, double& x2, double& y2) {
    // Convert angle from degrees to radians
    double angleRadians = angleDegrees * M_PI / 180.0;

    // Calculate the destination coordinates
    x2 = x1 + distance * cos(angleRadians);
    y2 = y1 + distance * sin(angleRadians);
}

extern SDL_Window* win;
#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[BULLET][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[BULLET][ERROR]: " << A << "\n";
//#define DRAW_VECTOR_BULLET_TRAIL
namespace bullet {
  static constexpr int INITIAL_POINTS = 16; // TODO: when WIN_WIDTH is higher, this should be lower
  static std::unique_ptr<BulletPool> pool = nullptr;
  static Actor bullet_trail;
  static Actor shotgun_shell;
  static constexpr double PI = 3.14159265358979323846;
  static Actor mp5;
  static Actor p226;
  static constexpr size_t SHOTGUN_MIN = 6;
  static constexpr size_t SHOTGUN_MAX = 12;
  static constexpr std::size_t MAX_SHELL_POSITIONS = 128;
  static SDL_mutex* shell_positions_mutex = SDL_CreateMutex();
  static constexpr std::size_t DIRECTIONS_COUNT = 8;
	static constexpr std::array<int,DIRECTIONS_COUNT> direction_list = {
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NORTH_EAST,
	NORTH_WEST,
	SOUTH_EAST,
	SOUTH_WEST,
  };
  template <typename TDamageType>
  struct damage_display_t {
    SDL_Point where;
    TDamageType damage_amount;
    uint64_t display_until;
  };
  struct shell_position_t {
    SDL_Rect where;
    int angle;
    int trajectory;
    int velocity;
    wpn::weapon_t type;
    bool draw;
    uint64_t created_at;
    uint64_t trajectory_change_at;
  };
  static std::vector<damage_display_t<Player::gun_damage_t>> damage_display_list;
  static std::array<shell_position_t,MAX_SHELL_POSITIONS> shell_positions;
  static size_t shell_positions_index = 0;
  static size_t mag_positions_index = 0;
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
  }
  void Bullet::calc() {
    done = true;
    trimmed.clear();
    line.points.clear();

    distance = closest = 9999;
    line_index = 0;

	  angle = angleBetweenPoints(src.x,src.y,dst.x,dst.y);

#ifdef NO_SHOTGUN_RECOIL
#else
    auto tmp_angle = angle;
    if(is_shotgun((*stats)[WPN_TYPE])) { //|| ((*stats)[WPN_ACCURACY] < 100 && (*stats)[WPN_ACCURACY] < rand_between(1,100))){
      if(rand_between(1,256) % 2){
        angle += rand_between((*stats)[WPN_ACCURACY_DEVIATION_START],(*stats)[WPN_ACCURACY_DEVIATION_END]);
      }else{
        angle -= rand_between((*stats)[WPN_ACCURACY_DEVIATION_START],(*stats)[WPN_ACCURACY_DEVIATION_END]);
      }
      if(rng::chance(10)){
        angle = tmp_angle;
      }
    }
#endif
#ifdef NO_WEAPON_ACCURACY
#else
    auto tmp_angle = angle;
    if(((*stats)[WPN_ACCURACY] < 100 && (*stats)[WPN_ACCURACY] < rand_between(1,100))){
      if(rand_between(1,256) % 2){
        angle += rand_between((*stats)[WPN_ACCURACY_DEVIATION_START],(*stats)[WPN_ACCURACY_DEVIATION_END]);
      }else{
        angle -= rand_between((*stats)[WPN_ACCURACY_DEVIATION_START],(*stats)[WPN_ACCURACY_DEVIATION_END]);
      }
      if(rng::chance(10)){
        angle = tmp_angle;
      }
    }
#endif
    line.p1.x = src.x;
    line.p1.y = src.y;

    double x2, y2;
	  calculateDestination(src.x, src.y, angle, rand_between(1024,2048), x2, y2);
    line.p2.x = x2;
    line.p2.y = y2;

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
    done = false;
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
    if(line_index >= trimmed.size() - 1 || trimmed.size() == 0) {
      done = true;
      trimmed.clear();
      line.points.clear();
      return;
    }
    rect.x = trimmed[line_index].x;
    rect.y = trimmed[line_index].y;
    SDL_Rect result;

    bool impact = 0;
    bool hits_player = SDL_IntersectRect(
            &rect,
            plr::get_rect(),
            &result);
    if(queue_type == QUEUE_TYPE_NPC && hits_player){
        plr::take_damage(stats);
        impact = 1;
    } 
    if(queue_type == QUEUE_TYPE_PLAYER || queue_type == QUEUE_TYPE_TURRET){
      for(auto& npc : world->npcs) {
        if(SDL_IntersectRect(
              &rect,
              &npc->rect,
              &result)) {
          if(npc::is_dead(npc) || npc::bomber::is_dead(npc)){
            continue;
          }
          using gd_slots = Player::gun_damage_t;
          gd_slots p;
          if(queue_type == QUEUE_TYPE_TURRET){
            p = abilities::turret::gun_damage(npc_id);
          }else if(queue_type == QUEUE_TYPE_PLAYER){
            p = plr::gun_damage();
          }
          damage_display_list.emplace_back(SDL_Point{npc->rect.x,npc->rect.y},p,tick::get() + 2500);
          npc::take_damage(npc,p[0] + p[1] + p[2]);
          npc::bomber::take_damage(npc,p[0] + p[1] + p[2]); // TODO: handle other types of dmg
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
      done = true;
      trimmed.clear();
      line.points.clear();
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
    bullets[index]->queue_type = Bullet::queue_type_t::QUEUE_TYPE_PLAYER;
    bullets[index]->is_npc = false;
    bullets[index]->calc();
    ++index;
  }
  void BulletPool::queue_custom(weapon_stats_t* stats_ptr,Bullet::queue_type_t t,int in_cx,int in_cy,int in_dst_x,int in_dst_y,npc_id_t in_npc_id) {
    if(index >= POOL_SIZE) {
      index = 0;
    }

    if(bullets[index] == nullptr){
      bullets[index] = std::make_unique<Bullet>();
    }
    bullets[index]->stats = stats_ptr;
    bullets[index]->src.x = in_cx;
    bullets[index]->src.y = in_cy;
    bullets[index]->dst.x = in_dst_x;
    bullets[index]->dst.y = in_dst_y;
    bullets[index]->queue_type = t;
    bullets[index]->is_npc = t == Bullet::queue_type_t::QUEUE_TYPE_NPC;
    bullets[index]->npc_id = in_npc_id;
    bullets[index]->calc();
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
    bullets[index]->queue_type = Bullet::queue_type_t::QUEUE_TYPE_NPC;
    bullets[index]->stats = stats_ptr;
    bullets[index]->src.x = in_cx;
    bullets[index]->src.y = in_cy;
    bullets[index]->dst.x = dest_x;
    bullets[index]->dst.y = dest_y;
    bullets[index]->calc();
    ++index;
  }
  void queue_bullets(weapon_stats_t* stats_ptr) {
    if(!pool){
      m_debug("queue_bullets stats_ptr encounted a null pool!");
      pool = std::make_unique<BulletPool>();
    }
    if(is_shotgun((*stats_ptr)[WPN_TYPE])){
      for(size_t i=0; i < rand_between(SHOTGUN_MIN,SHOTGUN_MAX);i++){
        pool->queue(stats_ptr);
      }
    }else{
      pool->queue(stats_ptr);
    }
  }
  void queue_custom(weapon_stats_t* stats_ptr,Bullet::queue_type_t t,int in_cx,int in_cy,int in_dst_x,int in_dst_y,npc_id_t in_npc_id) {
    if(!pool){
      m_debug("queue_bullets stats_ptr encounted a null pool!");
      pool = std::make_unique<BulletPool>();
    }
    if(is_shotgun((*stats_ptr)[WPN_TYPE])){
      for(size_t i=0; i < rand_between(SHOTGUN_MIN,SHOTGUN_MAX);i++){
        pool->queue_custom(stats_ptr,t,in_cx,in_cy,in_dst_x,in_dst_y,in_npc_id);
      }
    }else{
      pool->queue_custom(stats_ptr,t,in_cx,in_cy,in_dst_x,in_dst_y,in_npc_id);
    }
  }
  void queue_npc_bullets(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx,int in_cy,int dest_x, int dest_y) {
    if(!pool){
      m_debug("queue_npc_bullets encounted a null pool!");
      pool = std::make_unique<BulletPool>();
    }
    if(is_shotgun((*stats_ptr)[WPN_TYPE])){
      for(size_t i=0; i < rand_between(SHOTGUN_MIN,SHOTGUN_MAX);i++){
        pool->queue_npc(in_npc_id,stats_ptr,in_cx,in_cx,dest_x,dest_y);
      }
    }else{
      pool->queue_npc(in_npc_id,stats_ptr,in_cx,in_cy,dest_x,dest_y);
    }
  }
  void draw_ammo() {
    if(!draw_state::ammo::draw_ammo()) {
      return;
    }
    static SDL_Point where{0,0};
    static uint16_t height = 25; // TODO: scale using window resolution
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
  void draw_shell_at(const uint32_t& x,const uint32_t& y,const uint32_t& wpn_type){
    if(shell_positions_index >= MAX_SHELL_POSITIONS){
      shell_positions_index = 0;
    }
    shell_positions[shell_positions_index].where.x = x;
    shell_positions[shell_positions_index].where.y = y;
    shell_positions[shell_positions_index].where.w = 22;
    shell_positions[shell_positions_index].where.h = 8;
    shell_positions[shell_positions_index].trajectory = direction_list[rand_between(1,10) % DIRECTIONS_COUNT];
    shell_positions[shell_positions_index].velocity = rand_between(5,15);
    shell_positions[shell_positions_index].angle = rand_between(1,360);
    shell_positions[shell_positions_index].type = (wpn::weapon_t)wpn_type;
    shell_positions[shell_positions_index].draw = true;
    shell_positions[shell_positions_index].created_at = tick::get();
    ++shell_positions_index;
  }
  void draw_shells(){
    auto i = tick::get();
    SDL_Rect r;
    auto start = rand_between(3,10);
    auto end = rand_between(13,23);
    for(auto& shell : shell_positions){
      if(shell.draw == false){
        continue;
      }
      if(is_shotgun(shell.type)){
        if(shell.velocity > 0){
          shell.velocity -= 1;
          switch(shell.trajectory){
            case SOUTH:
              shell.where.y += rand_between(start,end);
              if(rng::chaos()){
                shell.where.x += rand_between(start,end);
              }
              if(rng::chaos()){
                shell.where.x -= rand_between(start,end);
              }
              break;
            case WEST:
              shell.where.x -= rand_between(start,end);
              if(rng::chaos()){
                shell.where.y += rand_between(start,end);
              }
              if(rng::chaos()){
                shell.where.y -= rand_between(start,end);
              }
              break;
            case EAST:
              shell.where.x += rand_between(start,end);
              if(rng::chaos()){
                shell.where.y += rand_between(start,end);
              }
              if(rng::chaos()){
                shell.where.y -= rand_between(start,end);
              }
              break;
            case NORTH:
              shell.where.y -= rand_between(start,end);
              if(rng::chaos()){
                shell.where.x += rand_between(start,end);
              }
              if(rng::chaos()){
                shell.where.x -= rand_between(start,end);
              }
              break;
            case NORTH_EAST:
              shell.where.y -= rand_between(start,end);
              shell.where.x += rand_between(start,end);
              break;
            case NORTH_WEST:
              shell.where.y -= rand_between(start,end);
              shell.where.x -= rand_between(start,end);
              break;
            case SOUTH_WEST:
              shell.where.y += rand_between(start,end);
              shell.where.x -= rand_between(start,end);
              break;
            case SOUTH_EAST:
              shell.where.y += rand_between(start,end);
              shell.where.x += rand_between(start,end);
              break;
            default:
              shell.where.y += rand_between(start,end);
              shell.where.x -= rand_between(start,end);
              break;
          }
          shell.angle += rand_between(10,18);
          if(shell.trajectory_change_at + 30 < i){
            for(const auto& w : wall::blockable_walls){
              if(SDL_IntersectRect(&shell.where,&w->rect,&r)){
                shell.trajectory_change_at = tick::get();
                switch(shell.trajectory){
                  case NORTH:
                    if(w->rect.y < shell.where.y){
                      if(rng::chaos()){
                        shell.trajectory = SOUTH_WEST;
                      }
                      if(rng::chaos()){
                        shell.trajectory = SOUTH_EAST;
                      }
                    }
                    break;
                  case NORTH_EAST:
                    if(w->rect.y < shell.where.y || w->rect.x > shell.where.x){
                      shell.trajectory = SOUTH_EAST;
                    }else{
                      shell.trajectory = SOUTH_WEST;
                    }
                    break;
                  case NORTH_WEST:
                    if(w->rect.y < shell.where.y || w->rect.x < shell.where.x){
                      shell.trajectory = SOUTH_WEST;
                    }else{
                      shell.trajectory = SOUTH_EAST;
                    }
                    break;
                  case SOUTH_EAST:
                    if(w->rect.y > shell.where.y || w->rect.x > shell.where.x){
                      shell.trajectory = SOUTH_WEST;
                    }else{
                      shell.trajectory = NORTH_EAST;
                    }
                    break;
                  case SOUTH_WEST:
                    if(w->rect.y > shell.where.y || w->rect.x < shell.where.x){
                      shell.trajectory = NORTH_WEST;
                    }else{
                      shell.trajectory = NORTH_EAST;
                    }
                    break;
                  default:
                    break;
                }
              }
            }
          }
        }
        SDL_RenderCopyEx(
            ren,  //renderer
            shotgun_shell.bmp[0].texture,
            nullptr,// src rect
            &shell.where, // dst rect
            shell.angle, // angle
            nullptr,  // center
            SDL_FLIP_NONE // flip
            );
      }
      if(shell.created_at + 60 * 1000 < i){
        shell.draw = false;
      }
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
          std::to_string(pair[0]),    //const std::string& msg,
          20,//const uint16_t& height,
          50//const uint16_t& width);
      );
      if(pair[1] > 0){ // crit
        display.y = damage_display.where.y + 50;
        font::green_text(&display, //const SDL_Point* where,
            std::to_string(pair[1]),    //const std::string& msg,
            40,//const uint16_t& height,
            80//const uint16_t& width);
        );
      }
      if(pair[2] > 0){ // mega-crit
        display.y = damage_display.where.y + 50;
        display.x -= 50;
        font::green_text(&display, //const SDL_Point* where,
            std::to_string(pair[2]),    //const std::string& msg,
            80,//const uint16_t& height,
            130//const uint16_t& width);
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
      if(pool->bullets[i]->done == false){
        pool->bullets[i]->travel();
      }
      if(pool->bullets[i]->done) {
        continue;
      }
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
      angle -= 1;
      dest.x = (900 * win_width()) * cos(PI * 2  * angle / 360);
      dest.y = (900 * win_height()) * sin(PI * 2 * angle / 360);
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
          npc::take_damage(npc,p[0] + p[1] + p[2]);
        }
      }
    }
  }
  void init() {
    bullet_trail.x = 0;
    bullet_trail.y = 0;
    bullet_trail.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
    shotgun_shell.load_bmp_asset("../assets/shotgun-shell.bmp");
    mp5.load_bmp_asset("../assets/mp5.bmp");
    p226.load_bmp_asset("../assets/p226.bmp");
    radius = 55;
    pool = std::make_unique<BulletPool>();
    shell_positions_index = 0;
  }
  void cleanup_pool() {
  }
  void program_exit(){
  }
  void move_map(int dir,int amount){
    LOCK_MUTEX(shell_positions_mutex);
    for(auto& l: shell_positions){
      if(l.draw == false){
        continue;
      }
      switch(dir) {
        case NORTH_EAST:
          l.where.y += amount;
          l.where.x -= amount;
          break;
        case NORTH_WEST:
          l.where.y += amount;
          l.where.x += amount;
          break;
        case NORTH:
          l.where.y += amount;
          break;
        case SOUTH_EAST:
          l.where.y -= amount;
          l.where.x -= amount;
          break;
        case SOUTH_WEST:
          l.where.y -= amount;
          l.where.x += amount;
          break;
        case SOUTH:
          l.where.y -= amount;
          break;
        case WEST:
          l.where.x += amount;
          break;
        case EAST:
          l.where.x -= amount;
          break;
        default:
          break;
      }
    }
    UNLOCK_MUTEX(shell_positions_mutex);
  }
  };
#undef m_debug
