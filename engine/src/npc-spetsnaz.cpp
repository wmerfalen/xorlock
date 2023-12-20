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
#include "events/death.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[DEBUG]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";
#define m_error(A) std::cout << "[ERROR]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";

#define USE_PATH_TESTING_NORTH_EAST

//#define USE_PATH_TESTING_SOUTH_EAST

namespace npc {
  static bool halt_spetsnaz = false;
  static constexpr std::size_t SPETSNAZ_QUOTA = 10;
  static constexpr std::size_t INITIALIZE_SPETSNAZ =3;
  static constexpr uint32_t SPETSNAZ_CALL_COUNT = 180;
  static constexpr int SPETSNAZ_ADJUSTMENT_MULTIPLIER = 1.0;
  static constexpr std::size_t SPETS_WIDTH = 80;
  static constexpr std::size_t SPETS_HEIGHT = 53;
  static constexpr std::size_t SPETS_MOVEMENT = 2;
  static constexpr std::size_t BULLET_POOL_SIZE = 1024;
  static constexpr const char* BMP = "../assets/spet-0.bmp";
  static constexpr const char* HURT_BMP = "../assets/spet-hurt-%d.bmp";
  static constexpr std::size_t HURT_BMP_COUNT = 3;
  static constexpr const char* DEAD_BMP = "../assets/spet-dead-%d.bmp";
  static constexpr std::size_t DEAD_BMP_COUNT = 1;
  static constexpr const char* DETONATED_BMP = "../assets/spet-detonated-arm-%d.bmp";
  static constexpr std::size_t DETONATED_BMP_COUNT= 6;
  static constexpr int CENTER_X_OFFSET = 110;
  static constexpr uint16_t COOLDOWN_BETWEEN_SHOTS = 810;
  static constexpr float AIMING_RANGE_MULTIPLIER = 1.604;
  static constexpr uint16_t STUNNED_TICKS = 300;
  static constexpr const char* SPLATTERED_BMP = "../assets/spet-dead-splattered-0.bmp";

  static constexpr int SPETSNAZ_MAX_HP = 100;
  static constexpr int SPETSNAZ_LOW_HP = 75;
  static constexpr int SPETSNAZ_RANDOM_LO = 10;
  static constexpr int SPETSNAZ_RANDOM_HI = 25;
  static constexpr int SEE_DISTANCE = 500;

  static std::vector<Actor*> dead_list;
  static SDL_mutex* body_parts_mutex = SDL_CreateMutex();
  struct custom_asset {
    SDL_Surface* surface;
    SDL_Texture* texture;
    double angle;
    bool dispose;
    SDL_RendererFlip flip;
  };

  static std::forward_list<std::pair<std::unique_ptr<custom_asset>,SDL_Rect>> body_parts;
  static std::unique_ptr<Actor> detonated_actor = nullptr;
  static std::unique_ptr<Actor> splattered_actor = nullptr;
  static constexpr std::size_t FLIP_SIZE = 4;
  static constexpr std::array<SDL_RendererFlip,FLIP_SIZE> flip_values = {
    SDL_FLIP_NONE,
    SDL_FLIP_HORIZONTAL,
    SDL_FLIP_VERTICAL,
    (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL),
  };
  std::size_t dead_counter;
  std::size_t alive_counter;

  std::unique_ptr<custom_asset> random_detonated_asset(){
    auto p = std::make_unique<custom_asset>();
    auto index = rand_between(0,DETONATED_BMP_COUNT - 1);
    p->surface = detonated_actor->bmp[index].surface;
    p->texture = detonated_actor->bmp[index].texture;
    p->angle = rand_between(0,360);
    p->dispose = false;
    p->flip = flip_values[rand_between(0,FLIP_SIZE - 1)];
    return p;
  }

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
    if(halt_spetsnaz){
      return ;
    }
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
    if(halt_spetsnaz){
      return ;
    }
    auto i = rand_between(0,10);
    spawn_spetsnaz((1024 / 2) + (i * CELL_WIDTH), (1024 / 2) - (i * CELL_HEIGHT));
  }
  void init_spetsnaz() {
    if(halt_spetsnaz){
      return ;
    }
    detonated_actor = std::make_unique<Actor>();
    auto p = detonated_actor->load_bmp_assets(DETONATED_BMP,DETONATED_BMP_COUNT);
    m_debug("[DETONATED_ACTOR]: " << p.first << " bmps loaded successfully. " << p.second << " bmps FAILED to load.\n");

    splattered_actor = std::make_unique<Actor>();
    // TODO: load more than just this single bmp
    splattered_actor->load_bmp_asset(SPLATTERED_BMP);

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
    if(last_vocal + rand_between(1000,8000) < tick::get()){
      target_acquired();
      last_vocal = tick::get() + rand_between(rand_between(1,4) * 1000,rand_between(5,8) * 1000);
    }else{
      last_vocal = tick::get() + (rand_between(1,8) * 1000);
    }
  }
  void Spetsnaz::die(){
    m_debug("DIED");
    sound::npc::play_death_sound(Spetsnaz::TYPE_ID);
    events::death::dispatch(Spetsnaz::TYPE_ID,id,cx,cy);
  }
  bool Spetsnaz::dead(){
    return hp <= 0;
  }
  void Spetsnaz::corpse_hit(){
    sound::npc::play_corpse_sound(Spetsnaz::TYPE_ID,hp);
  }
  void Spetsnaz::take_damage(int damage) {
    if(dead()){
      corpse_hit();
      return;
    }

    hp -= damage;
    //std::cout << "spetsnaz[" << id << "](hp:" << hp << ") took " << damage << " damage ";
    if(hp <= 0){
      //std::cout << ".. and dies\n";
      dismembered = false;
      die();
      hp = 0;
      self.bmp[0] = dead_actor.self.bmp[rand_between(0,dead_actor.self.bmp.size()-1)];
      dead_list.emplace_back(&self);
      return;
    }
    sound::npc::play_npc_pain(Spetsnaz::TYPE_ID);
    self.bmp[0] = *next_state();
    m_stunned_until = STUNNED_TICKS + rand_between(200,500) + tick::get();
    //std::cout << ". hp after: " << hp << "\n";
  }
  void Spetsnaz::target_acquired(){
    sound::npc::play_intimidate_sound(Spetsnaz::TYPE_ID);
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
    if(halt_spetsnaz){
      return false;
    }
    vpair_t s{self.rect.x,self.rect.y};
    auto tile = paths::get_tile(s);
    vpair_t p{plr::self()->rect.x,plr::self()->rect.y};
    auto ptile = paths::get_tile(p);
    return paths::has_line_of_sight(tile,ptile);
  }
  void Spetsnaz::walk_to_next_path() {
    if(halt_spetsnaz){
      return;
    }
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
    if(halt_spetsnaz){
      return;
    }
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
  const std::size_t& dead_count() {
    return dead_counter;
  }
  const std::size_t& alive_count() {
    return alive_counter;
  }
  void spetsnaz_tick() {
    if(halt_spetsnaz){
      return;
    }
    size_t ctr=0;
    // TODO: create a mechanism that allows a texture to travel
    // at a variable speed where it has a source and a destination.
    // 
    for(auto& r : body_parts){
      //m_debug("drawing body_parts: " << ctr++);
      int i = SDL_RenderCopyEx(
          ren,  //renderer
          r.first->texture,
          nullptr,// src rect
          &r.second,
          r.first->angle, // angle
          nullptr,  // center
          r.first->flip // flip
          );
      if(0 != i){
        m_error("body_parts could not be drawn: " << SDL_GetError());
        r.first->dispose = true;
      }
    }
    body_parts.remove_if([](const auto& p) { return p.first->dispose; });
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
    if(halt_spetsnaz){
      return;
    }
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
    if(dismembered){
      return &splattered_actor->bmp[0];
    }
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
    if(halt_spetsnaz){
      return;
    }
    dismembered = false;
    ready = false;
    last_aim_tick = tick::get();
    call_count = 0;
    next_path = {0,0};
    path_finder = std::make_unique<npc::paths::PathFinder>(SPETS_MOVEMENT,&self,plr::self());
    last_vocal = tick::get();
  }
  Spetsnaz::Spetsnaz(const int32_t& _x,
      const int32_t& _y,
      const int& _ma,
      const npc_id_t& _id) {
    if(halt_spetsnaz){
      return;
    }
    dismembered = false;
    call_count = 0;
    self.rect.x = _x;
    self.rect.y = _y;
    self.rect.w = SPETS_WIDTH;
    self.rect.h = SPETS_HEIGHT;
    movement_amount = _ma;
    self.load_bmp_asset(BMP);
    path_finder = std::make_unique<npc::paths::PathFinder>(SPETS_MOVEMENT,&self,plr::self());

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
    last_vocal = tick::get();
  }
  void take_damage(Actor* a,int dmg) {
    if(halt_spetsnaz){
      return;
    }
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
    std::size_t size = 0;
    for(auto& sp : spetsnaz_list) {
      ++size;
      if(sp.is_dead()) {
        corpse_actors.emplace_back(&sp.self);
      }
    }
    dead_list.clear();
    if(corpse_actors.size()) {
      cleanup_dead_npcs(corpse_actors);
    }
    m_debug("cleanup_corpses spetsnaz_list size: " << size);
    spetsnaz_list.remove_if([&](const auto& sp) -> bool {
        return sp.is_dead();
        });
    size = 0;
    for(auto& sp : spetsnaz_list) {
      ++size;
    }
    m_debug("AFTER cleanup_corpses spetsnaz_list size: " << size);
  }
  Spetsnaz::~Spetsnaz(){
    states.clear();
    path_finder = nullptr;
  }
  void Spetsnaz::cleanup(){
    states.clear();
    path_finder = nullptr;
  }
  void program_exit(){
    halt_spetsnaz = true;
    for(auto& s : spetsnaz_list){
      s.cleanup();
    }
    spetsnaz_list.clear();
    dead_list.clear();
  }
  void Spetsnaz::take_explosive_damage(int damage,SDL_Rect* source_explosion,int blast_radius, int on_death,SDL_Rect* source_rect){
    // TODO: determine which direction to scatter body parts using source_explosion
    // TODO: calculate damage according to explosive velocity (damage and blast_radius)
    if(dead()){
      // TODO: splatter into even smaller bits and pieces
      return;
    }
    hp -= damage;
    if (hp <= 0){
      die();
      dismembered = false;
    }
    if(dead() && !dismembered){
      self.bmp[0] = splattered_actor->bmp[0];
      dead_actor.self.bmp[0] = splattered_actor->bmp[0];
      m_debug("creating body parts");
      std::pair<std::unique_ptr<custom_asset>,SDL_Rect> r;
      calc();
      r.second.x = self.rect.x;
      r.second.y = self.rect.y;
      SDL_Rect f;
      if(source_rect){
        f = *source_rect;
      }else{
        f = *source_explosion;
      }
      if(f.x < cx){
        // push east
        r.second.x += blast_radius + rand_between(10,50);
      }else if(f.x > cx){
        // push west
        r.second.x -= blast_radius + rand_between(10,50);
      }
      if(f.y < cy){
        // push south
        r.second.y += blast_radius + rand_between(10,50);
      }else if(f.y > cy){
        // push north
        r.second.y -= blast_radius + rand_between(10,50);
      }
      r.second.w = 40;
      r.second.h = 40;
      r.first = random_detonated_asset();
      body_parts.emplace_front(std::move(r));
      dismembered = true;
    }
  }
  void take_explosive_damage(Actor* a, int damage,SDL_Rect* source_explosion,int blast_radius, int on_death,SDL_Rect* src_rect){
    for(auto& s : spetsnaz_list) {
      if(&s.self == a) {
        s.take_explosive_damage(damage,source_explosion,blast_radius,on_death,src_rect);
      }
    }
  }
  void move_map(int dir, int amount){
    if(halt_spetsnaz){
      return;
    }
    LOCK_MUTEX(body_parts_mutex);
    for(auto& exp : body_parts){
      switch(dir) {
        case NORTH_EAST:
          exp.second.y += amount;
          exp.second.x -= amount;
          break;
        case NORTH_WEST:
          exp.second.y += amount;
          exp.second.x += amount;
          break;
        case NORTH:
          exp.second.y += amount;
          break;
        case SOUTH_EAST:
          exp.second.y -= amount;
          exp.second.x -= amount;
          break;
        case SOUTH_WEST:
          exp.second.y -= amount;
          exp.second.x += amount;
          break;
        case SOUTH:
          exp.second.y -= amount;
          break;
        case WEST:
          exp.second.x += amount;
          break;
        case EAST:
          exp.second.x -= amount;
          break;
        default:
          break;
      }
    }
    UNLOCK_MUTEX(body_parts_mutex);
  }
};

#undef m_debug
