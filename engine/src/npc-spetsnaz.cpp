#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "npc-spetsnaz.hpp"
#include "player.hpp"
#include "direction.hpp"
#include "npc/paths.hpp"
#include "font.hpp"
#include "colors.hpp"
#include "rng.hpp"
#include "sound/gunshot.hpp"
#include "sound/npc.hpp"

//#define NPC_SPETSNAZ_DEBUG
#ifdef NPC_SPETSNAZ_DEBUG
#define m_debug(A) std::cerr << "[DEBUG]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";
#else
#define m_debug(A)
#endif

#define USE_PATH_TESTING_NORTH_EAST

//#define USE_PATH_TESTING_SOUTH_EAST

namespace npc {
  static constexpr std::size_t SPETSNAZ_QUOTA = 10;
  static constexpr std::size_t INITIALIZE_SPETSNAZ =3;

  bool Spetsnaz::within_range() {
    calc();
    auto& px = plr::get_cx();
    return px <= cx + center_x_offset() && px >= cx - center_x_offset();
  }
  void Spetsnaz::calculate_aim() {
    target_x = plr::get_cx();
    target_y = plr::get_cy();
  }
  void spawn_spetsnaz(const int& in_start_x, const int& in_start_y) {
    auto tile = npc::paths::get_tile(vpair_t{in_start_x,in_start_y});
    if(tile) {
      m_debug("found tile");
      spetsnaz_list.emplace_front(tile->rect.x,tile->rect.y,SPETS_MOVEMENT,npc_id::next());
      world->npcs.push_front(&spetsnaz_list.front().self);
      return;
    }
    m_debug("possibly not a good tile!");
    spetsnaz_list.emplace_front(in_start_x,in_start_y,SPETS_MOVEMENT,npc_id::next());
    world->npcs.push_front(&spetsnaz_list.front().self);
  }
  void spawn_spetsnaz() {
    auto i = rand_between(0,10);
    spawn_spetsnaz((1024 / 2) + (i * CELL_WIDTH), (1024 / 2) - (i * CELL_HEIGHT));
  }
  void init_spetsnaz() {
#ifdef USE_PATH_TESTING_SOUTH_EAST
    spawn_spetsnaz((1024 / 4), (1024 / 128) - 280);
#endif
#ifdef USE_PATH_TESTING_NORTH_EAST
    //spawn_spetsnaz((1024 / 2), (1024 / 2) + 280);
    spawn_spetsnaz(1580, 210);
#endif
#ifdef USE_PRODUCTION_SPETSNAZ
    for(auto i=0; i < INITIALIZE_SPETSNAZ; i++) {
      spawn_spetsnaz();
    }
#endif
  }
  uint16_t Spetsnaz::cooldown_between_shots() {
    return COOLDOWN_BETWEEN_SHOTS;
  }
  bool Spetsnaz::can_fire_again() {
    return m_last_fire_tick + cooldown_between_shots() <= tick::get();
  }
  void Spetsnaz::fire_at_player() {
    sound::play_mp5_gunshot();
    m_last_fire_tick = tick::get();
    plr::calc();
    calc();
#ifdef DRAW_SPETSNAZ_PREFIRE_LINE
    draw::line(cx,cy,target_x,target_y);
#endif

    bullet::queue_npc_bullets(id,weapon_stats(),cx,cy,target_x,target_y);
  }
  void Spetsnaz::aim_at_player() {
    draw::line(cx,cy,target_x,target_y);
  }
  void Spetsnaz::die(){
    sound::npc::play_death_sound(Spetsnaz::TYPE_ID);
  }
  bool Spetsnaz::dead(){
    return hp <= 0;
  }
  void Spetsnaz::corpse_hit(){
    sound::npc::play_corpse_sound(Spetsnaz::TYPE_ID,hp);
  }
  void Spetsnaz::take_damage(int damage) {
    if(dead()){
      return;
    }
    
    hp -= damage;
    if(hp <= 0){
      die();
      hp = 0;
      self.bmp[0] = dead_actor.self.bmp[rand_between(0,dead_actor.self.bmp.size()-1)];
      dead_list.emplace_back(&self);
      return;
    }
    sound::npc::play_npc_pain(Spetsnaz::TYPE_ID);
    self.bmp[0] = *next_state();
    m_stunned_until = STUNNED_TICKS + rand_between(200,500) + tick::get();
  }
  float Spetsnaz::aiming_range_multiplier() {
    /**
     * TODO: add randomness
     */
    return AIMING_RANGE_MULTIPLIER;
  }
  bool Spetsnaz::within_aiming_range() {
    return npc::paths::distance(&self,plr::self()) < SEE_DISTANCE;
  }
  void Spetsnaz::move_to(SDL_Point* in_point) {
    if(in_point) {
      self.rect.x = in_point->x;
      self.rect.y = in_point->y;
    }
  }
  void Spetsnaz::move_to(const int32_t& x,const int32_t& y) {
    self.rect.x = x;
    self.rect.y = y;
  }
  void Spetsnaz::show_confused() {
    SDL_Point p;
    p.x = self.rect.x;
    p.y = self.rect.y;
    p.x += 90;
    p.y -= 100;
    draw::bubble_text(&p,"huh?!?!");
  }
  bool Spetsnaz::can_see_player() {
    vpair_t s{self.rect.x,self.rect.y};
    auto tile = paths::get_tile(s);
    vpair_t p{plr::self()->rect.x,plr::self()->rect.y};
    auto ptile = paths::get_tile(p);
    return paths::has_line_of_sight(tile,ptile);
  }
  void Spetsnaz::walk_to_next_path() {
    if(next_path.x < cx) {
      move_left();
    } else if(next_path.x > cx) {
      move_right();
    }
    if(next_path.y > cy) {
      move_south();
    } else if(next_path.y < cy) {
      move_north();
    }
    calc();
  }
  void Spetsnaz::perform_ai() {
    if(m_stunned_until > tick::get()) {
      return;
    }
    calc();
    update_check();
    walk_to_next_path();
    if(can_see_player()) {
      if(within_aiming_range()) {
        calculate_aim();
        aim_at_player();
      }
      if(within_range() && can_fire_again()) {
        fire_at_player();
      }
    }
  }
  std::size_t dead_counter;
  std::size_t alive_counter;
  const std::size_t& dead_count() {
    return dead_counter;
  }
  const std::size_t& alive_count() {
    return alive_counter;
  }
  void spetsnaz_tick() {
    dead_counter = alive_counter = 0;
    for(auto& s : spetsnaz_list) {
      if(s.is_dead()) {
        ++dead_counter;
      } else {
        ++alive_counter;
      }
      //++spetsnaz_count;
      s.tick();
#ifdef DRAW_PLR_TO_SPETSNAZ_LINE
      draw::line(s.self.rect.x, s.self.rect.y,plr::self()->x,plr::self()->y);
#endif
      SDL_RenderCopyEx(
          ren,  //renderer
          s.self.bmp[0].texture,
          nullptr,// src rect
          &s.self.rect,
          s.angle, // angle
          nullptr,  // center
          SDL_FLIP_NONE // flip
          );
    }
    //if(++call_count == 280) {
    //	if(spetsnaz_count < SPETSNAZ_QUOTA) {
    //		spawn_spetsnaz();
    //	}
    //	call_count = 0;
    //}
  }
  void spetsnaz_movement(uint8_t dir,int adjustment) {
    for(auto& s : spetsnaz_list) {
      if(s.is_dead()) {
        continue;
      }
      switch(Direction(dir)) {
        case NORTH_WEST:
          s.self.rect.x += abs(adjustment);
          s.self.rect.y += abs(adjustment);
          break;
        case NORTH_EAST:
          s.self.rect.x -= abs(adjustment);
          s.self.rect.y += abs(adjustment);
          break;
        case SOUTH_EAST:
          s.self.rect.x -= abs(adjustment);
          s.self.rect.y -= abs(adjustment);
          break;
        case SOUTH_WEST:
          s.self.rect.x += abs(adjustment);
          s.self.rect.y -= abs(adjustment);
          break;
        case WEST:
          s.self.rect.x += abs(adjustment);
          break;
        case EAST:
          s.self.rect.x -= abs(adjustment);
          break;
        case SOUTH:
          s.self.rect.y -= abs(adjustment);
          break;
        case NORTH:
          s.self.rect.y += abs(adjustment);
          break;
      }
      s.calc();
    }
  }
  SDL_Texture* Spetsnaz::initial_texture() {
    return self.bmp[0].texture;
  }
  void Spetsnaz::calc() {
    plr::calc();
    cx = self.rect.x + self.rect.w / 2;
    cy = self.rect.y + self.rect.h / 2;
    angle = coord::get_angle(cx,cy,plr::get_cx(),plr::get_cy());
  }
  void Spetsnaz::tick() {
    if(is_dead()) {
      return;
    }
    calc();
    perform_ai();
  }
  Asset* Spetsnaz::next_state() {
    if(hp <= 0) {
      return &dead_actor.self.bmp[0];
    }
    return states[0];
  }
  void Spetsnaz::move_south() {
    self.rect.y += movement_amount;
  }
  void Spetsnaz::move_north() {
    self.rect.y -= movement_amount;
  }

  void Spetsnaz::move_left() {
    self.rect.x -= movement_amount;
  }
  void Spetsnaz::move_right() {
    self.rect.x += movement_amount;
  }
  int Spetsnaz::center_x_offset() {
    return CENTER_X_OFFSET;
  }
  const bool Spetsnaz::is_dead() const {
    return hp <= 0;
  }
  uint32_t Spetsnaz::weapon_stat(WPN index) {
    return (*(mp5.stats))[index];
  }
  weapon_stats_t* Spetsnaz::weapon_stats() {
    return mp5.stats;
  }
  int Spetsnaz::gun_damage() {
    return rand_between(mp5.dmg_lo(),mp5.dmg_hi());
  }

  Spetsnaz::Spetsnaz() {
    ready = false;
    last_aim_tick = tick::get();
    call_count = 0;
    next_path = {0,0};
    path_finder = std::make_unique<npc::paths::PathFinder>(SPETS_MOVEMENT,&self,plr::self());
  }
  Spetsnaz::Spetsnaz(const int32_t& _x,
      const int32_t& _y,
      const int& _ma,
      const npc_id_t& _id) {
    call_count = 0;
    path_finder = std::make_unique<npc::paths::PathFinder>(SPETS_MOVEMENT,&self,plr::self());
    self.rect.x = _x;
    self.rect.y = _y;
    self.rect.w = SPETS_WIDTH;
    self.rect.h = SPETS_HEIGHT;
    movement_amount = _ma;
    self.load_bmp_asset(BMP);

    hurt_actor.self.load_bmp_assets(HURT_BMP,HURT_BMP_COUNT);
    dead_actor.self.load_bmp_assets(DEAD_BMP,DEAD_BMP_COUNT);
    hp = SPETSNAZ_LOW_HP;
    max_hp = SPETSNAZ_MAX_HP;
    ready = true;

    state_index = 0;
    for(int i=0; i < hurt_actor.self.bmp.size(); ++i) {
      states.emplace_back(&hurt_actor.self.bmp[i]);
    }
    id = _id;
    calc();
    m_last_fire_tick = 0;
    m_stunned_until = 0;
    last_aim_tick = tick::get();
    next_path = {self.rect.x,self.rect.y};
    move_to(_x,_y);
  }
  void take_damage(Actor* a,int dmg) {
    for(auto& s : spetsnaz_list) {
      if(&s.self == a) {
        s.take_damage(dmg);
      }
    }
  }
  bool is_dead(Actor* a) {
    return std::find(dead_list.cbegin(), dead_list.cend(), a) != dead_list.cend();
  }
  void Spetsnaz::update_check() {
    if(++call_count >= 80) {
      path_finder->update(&self,plr::self());
      auto point = path_finder->next_point();
      if(!point) {
        return;
      }
      next_path = *point;
      call_count = 0;
    }
  }
  void cleanup_corpses() {
    std::vector<Actor*> corpse_actors;
    for(auto& sp : spetsnaz_list) {
      if(sp.is_dead()) {
        corpse_actors.emplace_back(&sp.self);
      }
    }
    if(corpse_actors.size()) {
      cleanup_dead_npcs(corpse_actors);
    }
    spetsnaz_list.remove_if([&](const auto& sp) -> bool {
        return sp.is_dead();
        });
  }
};

#undef m_debug
