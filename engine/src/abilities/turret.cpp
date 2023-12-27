#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "turret.hpp"
#include "../player.hpp"
#include "../direction.hpp"
#include "../constants.hpp"
#include "../npc/paths.hpp"
#include "../font.hpp"
#include "../colors.hpp"
#include "../rng.hpp"
#include "../sound/gunshot.hpp"
#include "../sound/npc.hpp"
#include "../viewport.hpp"
#include "../damage/explosions.hpp"
#include "../sound/gunshot.hpp"
#include "../npc-bomber.hpp"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_mutex.h>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[AIR-SUPPORT][TURRET][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[AIR-SUPPORT][TURRET][ERROR]: " << A << "\n";
extern double angleBetweenPoints(double x1, double y1, double x2, double y2);

namespace abilities::turret {
  static bool halt_turret = false;
  static constexpr int TURRET_AUDIO_CHANNEL = 5;
  static constexpr std::size_t TURRET_WIDTH = 86;
  static constexpr std::size_t TURRET_HEIGHT = 120;
  static constexpr std::string_view BMP = "../assets/turret.bmp";
  static int mode = 0;
  static SDL_mutex* turret_list_mutex = SDL_CreateMutex();
  static SDL_mutex* mode_mutex = SDL_CreateMutex();
  static std::forward_list<std::unique_ptr<Turret>> turret_list;
  static constexpr int TURRET_IDLE = 0;
  static constexpr int TURRET_DEPLOY = 1;
  static constexpr int TURRET_HUNT = 2;
  static constexpr int TURRET_DESTROY = 3;
  weapon_stats_t gatling_stats = {
    0, //WPN_FLAGS = 0,
    wpn::weapon_t::WPN_GAU8A_AVENGER,
    480,//WPN_DMG_LO,
    490,//WPN_DMG_HI,
    3,//WPN_BURST_DLY,
    90,//WPN_PIXELS_PT,
    980,//WPN_CLIP_SZ,
    2890,//WPN_AMMO_MX,
    0,//WPN_RELOAD_TM,
    0,//WPN_COOLDOWN_BETWEEN_SHOTS,
    0,//WPN_MS_REGISTRATION,
    0,//WPN_MAG_EJECT_TICKS,
    0,//WPN_PULL_REPLACEMENT_MAG_TICKS,
    0,//WPN_LOADING_MAG_TICKS,
    0,//WPN_SLIDE_PULL_TICKS,
    0,//WPN_WIELD_TICKS,
    100,//WPN_ACCURACY,
    0,//WPN_ACCURACY_DEVIATION_START,
    0,//WPN_ACCURACY_DEVIATION_END,
  };
  /**
   * Constructor
   */
  Turret::Turret(const int32_t& _x, const int32_t& _y){
    npc_id = ::npc_id::next();
    gatling = std::make_unique<weapons::Primary>();
    gatling->feed(&gatling_stats);
    call_count = 0;
    self.rect.x = _x;
    self.rect.y = _y;
    self.rect.w = TURRET_WIDTH;
    self.rect.h = TURRET_HEIGHT;
    movement_amount = 450;
    self.load_bmp_asset(BMP.data());

    hp = 550;
    max_hp = 850;
    ready = true;

    state_index = 0;
    calc();
    m_last_fire_tick = 0;
    move_to(_x,_y);
    dispatched = true;
    sweep_forward = true;

    switch(dir::get_facing(plr::get()->angle)) {
      case Direction::SOUTH_EAST:
        /** Facing SOUTH EAST */
        start_angle = 360 - 45;
        break;
      case Direction::EAST:
        /** FACING EAST */
        start_angle = 360 - 90;
        break;
      case Direction::NORTH_EAST:
        /** FACING NORTH EAST */
        start_angle = 270 + 45;
        break;
      case Direction::NORTH:
        /** FACING NORTH */
        start_angle = 270 - 90;
        break;
      case Direction::NORTH_WEST:
        /** FACING NORTH WEST */
        start_angle = 180 - 45;
        break;
      case Direction::WEST:
        /** FACING WEST */
        start_angle = 180 - 90;
        break;
      case Direction::SOUTH_WEST:
        /** FACING SOUTH WEST */
        start_angle = 90;
        break;
      default:
      case Direction::SOUTH:
        /** FACING SOUTH */
        start_angle = 0;
        break;
    }

    target_angle = angle = start_angle;
    burst_count = 4;
    m_last_refill_tick = tick::get();
  }

  void move_map(int dir, int amount){
    if(halt_turret){
      return;
    }
    LOCK_MUTEX(turret_list_mutex);
    for(auto& t : turret_list){
      switch(dir) {
        case NORTH_EAST:
          t->self.rect.y += amount;
          t->self.rect.x -= amount;
          break;
        case NORTH_WEST:
          t->self.rect.y += amount;
          t->self.rect.x += amount;
          break;
        case NORTH:
          t->self.rect.y += amount;
          break;
        case SOUTH_EAST:
          t->self.rect.y -= amount;
          t->self.rect.x -= amount;
          break;
        case SOUTH_WEST:
          t->self.rect.y -= amount;
          t->self.rect.x += amount;
          break;
        case SOUTH:
          t->self.rect.y -= amount;
          break;
        case WEST:
          t->self.rect.x += amount;
          break;
        case EAST:
          t->self.rect.x -= amount;
          break;
        default:
          break;
      }
    }
    UNLOCK_MUTEX(turret_list_mutex);
  }

  static inline void cast_rays(){
    if(halt_turret){
      return;
    }
    const int& x = plr::cx();
    const int& y = plr::cy();
    SDL_Rect r;
    r.x = x + 50;
    r.y = y + 50;
    r.w = 50;
    r.h = 50;
    draw::blatant_rect(&r);
  }
  void Turret::calculate_aim() {
    target_x = plr::get_cx();
    target_y = plr::get_cy();
  }
  void spawn(const int& in_start_x, const int& in_start_y) {
    if(halt_turret){
      return;
    }
    LOCK_MUTEX(turret_list_mutex);
    turret_list.emplace_front(std::make_unique<Turret>(in_start_x,in_start_y));
    UNLOCK_MUTEX(turret_list_mutex);
  }
  bool accept_space_bar = true;
  void init() {
    LOCK_MUTEX(mode_mutex);
    mode = TURRET_IDLE;
    UNLOCK_MUTEX(mode_mutex);
  }
  void space_bar_pressed(){
    if(halt_turret){
      return;
    }

  }
  uint32_t Turret::cooldown_between_shots(){
    return 2000;
  }
  bool Turret::can_fire_again() {
    return m_last_fire_tick + cooldown_between_shots() <= tick::get();
  }
  void Turret::move_to(const int32_t& x,const int32_t& y) {
    self.rect.x = x;
    self.rect.y = y;
  }
  void Turret::perform_ai() {
#if defined(DO_SWEEP)
    if(sweep_forward){
      angle += 1;
      if(angle == start_angle + 90){
        sweep_forward = false;
      }
    }else{
      angle -= 1;
      if(angle == start_angle - 90){
        sweep_forward = true;
      }
    }
#endif
    if(angle == 450 && target_angle < 100){
      angle = target_angle;
    }
    if(angle < 100 && target_angle > 440){
      angle = target_angle;
    }
    if(angle > 440 && target_angle < 100){
      angle = target_angle;
    }
    if(angle != target_angle){
      if(angle > target_angle){
        angle -= 10;
        if(angle < target_angle){
          angle = target_angle;
        }
      }else{
        angle += 10;
        if(angle > target_angle){
          angle = target_angle;
        }
      }
    }
#ifdef TURRETS_ONLY_TARGET_PLAYER
    target_angle = rotate_towards(plr::get()->cx,plr::get()->cy);
    burst_fire_at(plr::get()->cx,plr::get()->cy);
    return;
#endif
    if(targets.size()){
      std::vector<npc_id_t> dead;
      for(const auto& s : npc::spetsnaz_list){
        if(std::find(targets.cbegin(),targets.cend(),s.id) != targets.cend()){
          // TODO: limit targets by distance. if too far away, turret cannot fire
          if(s.is_dead()){
            std::erase_if(targets,[&](auto& npc_id){ return npc_id == s.id;});
            continue;
          }
          target_angle = rotate_towards(s.self.rect.x,s.self.rect.y);
          burst_fire_at(s.self.rect.x,s.self.rect.y);
          return;
        }
      }
      for(const auto& s : npc::bomber::data::bomber_list){
        if(std::find(targets.cbegin(),targets.cend(),s.id) != targets.cend()){
          // TODO: limit targets by distance. if too far away, turret cannot fire
          if(s.is_dead()){
            std::erase_if(targets,[&](auto& npc_id){ return npc_id == s.id;});
            continue;
          }
          target_angle = rotate_towards(s.self.rect.x,s.self.rect.y);
          burst_fire_at(s.self.rect.x,s.self.rect.y);
          return;
        }
      }
      return;
    }

    for(const auto& s : npc::spetsnaz_list){
      if(s.is_dead()){
        continue;
      }
      if(npc::paths::has_line_of_sight(self.rect,s.self.rect)){
        if(std::find(targets.cbegin(),targets.cend(),s.id) == targets.cend()){
          targets.emplace_back(s.id);
          break;
        }
        burst_fire_at(target_x,target_y);
      }
    }
    for(const auto& s : npc::bomber::data::bomber_list){
      if(s.is_dead()){
        continue;
      }
      if(npc::paths::has_line_of_sight(self.rect,s.self.rect)){
        if(std::find(targets.cbegin(),targets.cend(),s.id) == targets.cend()){
          targets.emplace_back(s.id);
          return;
        }
        burst_fire_at(target_x,target_y);
      }
    }
  }
  void Turret::burst_fire_at(const int32_t& target_x,const int32_t& target_y){
    if(angle == target_angle && burst_count > 0){
      for(int i=burst_count; i > 0;--i){
        fire_at(target_x,target_y);
      }
    }
  }
  double Turret::rotate_towards(const int32_t& target_x,const int32_t& target_y){
    SDL_Rect adjusted = self.rect;
    double x1 = adjusted.x + TURRET_WIDTH / 2;
    double y1 = adjusted.y + TURRET_HEIGHT / 2;
    double x2 = target_x;
    double y2 = target_y;
    double tmp_angle = angleBetweenPoints(x1,y1,x2,y2);
    return tmp_angle + 270;
  }
  bool Turret::fire_at(const int32_t& target_x,const int32_t& target_y){
    SDL_Rect adjusted = self.rect;
    double x1 = adjusted.x + TURRET_WIDTH / 2;
    double y1 = adjusted.y + TURRET_HEIGHT / 2;
    double x2 = target_x;
    double y2 = target_y;
	  if(npc::paths::distance(x1,y1,x2,y2) > 750){
      return true;
    }
    bullet::queue_custom(gatling->stats,bullet::Bullet::queue_type_t::QUEUE_TYPE_TURRET,x1,y1,x2,y2,npc_id);
    sound::play_mg_gunshot();
    if(burst_count - 1 == 0){
      burst_count = 0;
      return false;
    }
    --burst_count;
    return burst_count > 0;
  }
  void tick() {
    if(halt_turret){
      return;
    }
    for(auto& s : turret_list) {
      if(!s->dispatched){
        continue;
      }
      s->tick();
      SDL_RenderCopyEx(
          ren,  //renderer
          s->self.bmp[0].texture,
          nullptr,// src rect
          &s->self.rect,
          s->angle, // angle
          nullptr,  // center
          SDL_FLIP_NONE // flip
          );
    }
  }
  SDL_Texture* Turret::initial_texture() {
    return self.bmp[0].texture;
  }
  void Turret::calc() {
    cx = self.rect.x + self.rect.w / 2;
    cy = self.rect.y + self.rect.h / 2;
    angle = 0; // FIXME
  }
  void Turret::tick() {
    if(m_last_refill_tick <= tick::get()){
      burst_count = 8;
      m_last_refill_tick = tick::get() + 1400;
    }
    perform_ai();
    SDL_RenderCopyEx(
        ren,  //renderer
        self.bmp[0].texture,
        nullptr,// src rect
        &self.rect,
        angle, // angle
        nullptr,  // center
        SDL_FLIP_NONE // flip
        );
  }
  Asset* Turret::next_state() {
    return states[0];
  }
  int Turret::gun_damage() {
    return rand_between(550,850);
  }

  void Turret::deploy_at(const SDL_Point& p){
    hp = 550;
    max_hp = 850;
    ready = true;

    state_index = 0;
    m_last_fire_tick = 0;
    move_to(p.x,p.y);
    calc();
    deployed_at = tick::get();
    deployment_location = p;
    dispatched = true;
  }
  void program_exit(){
    halt_turret = true;
    turret_list.clear();
  }
  gd_slots gun_damage(npc_id_t& npc_id){
    for(auto& turret : turret_list){
      if(turret->npc_id == npc_id){
        int d = rand_between(turret->gatling->stat(WPN_DMG_LO),turret->gatling->stat(WPN_DMG_HI));
        gd_slots m_gun_damage;
        std::fill(m_gun_damage.begin(),m_gun_damage.end(),0);
        int crit = 0;
        if(rng::chance(10)){
          m_gun_damage[1] = rand_between(turret->gatling->stat(WPN_DMG_HI), turret->gatling->stat(WPN_DMG_HI) * 2);
          m_debug("crit: " << m_gun_damage[1]);
        }else{
          m_gun_damage[0] = d;
        }
        if(rng::chance(1)){
          m_gun_damage[2] = rand_between(turret->gatling->stat(WPN_DMG_HI), turret->gatling->stat(WPN_DMG_HI) * 2);
          m_debug("MEGA CRIT: " << m_gun_damage[2]);
        }
        // TODO: explosive damage
        // TODO: shrapnel damage
        // TODO: incendiary damage
        return m_gun_damage;
      }
    }
    return {};
  }
};

#undef m_debug
