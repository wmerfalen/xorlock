#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "player.hpp"
#include "weapons.hpp"
#include "direction.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include <map>

#include "circle.hpp"
#include "npc-spetsnaz.hpp"
#include "weapons.hpp"
#include "weapons/pistol/p226.hpp"
#include "weapons/pistol.hpp"
#include "weapons/grenade.hpp"
#include "cursor.hpp"
#include "bullet.hpp"
#include "draw.hpp"
#include "font.hpp"
#include "colors.hpp"
#include "draw-state/player.hpp"
#include "draw-state/reticle.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[PLAYER][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[PLAYER][ERROR]: " << A << "\n";
static floatPoint top_right;
static constexpr int SCALE = 2;
static constexpr int W = 59 * SCALE;
static constexpr int H = 23 * SCALE;
Player::~Player(){
  reloader = nullptr;
  primary = nullptr;
  self = {};
}

void Player::weapon_click() {
	SDL_Point p{cx - 250,cy - 250};
	font::red_text(&p,"RELOAD",80,550);
}
void Player::consume_ammo() {
	if(ammo) {
#ifdef AMMO_CONSUMED
		(*ammo) -= AMMO_CONSUMED;
#else
		(*ammo) -= 1;
#endif
	}
}
Player::Player(int32_t _x,int32_t _y,const char* _bmp_path,int _base_movement_amount) :
	self(_x,_y,_bmp_path) {
    holding_grenade_at = 0;
#ifdef SHOW_PLR_DIMENSIONS
	std::cout << "W: " << W << "\n";
	std::cout << "H: " << H << "\n";
#endif
  changing_weapon = 0;
  has_target_at = 0;
	self.rect.w = W;
	self.rect.h = H;
	self.rect.x = _x;
	self.rect.y = _y;
	self.world_x = 0;
	self.world_y = 0;
	movement_amount = _base_movement_amount;

	firing_weapon = 0;
	hp = STARTING_HP;
	armor = STARTING_ARMOR;
	// TODO: load from file
  primary = std::make_unique<weapons::Primary>();
	pistol = std::make_unique<weapons::Pistol>();
  frag = std::make_unique<weapons::grenade::Frag>();
  target_equipped_weapon = -1;
  grenade_manager = std::make_unique<weapons::Grenade>();
  backpack = std::make_unique<backpack::Backpack>();
  backpack->load();
  weapons::grenade::register_traveler(grenade_manager.get());
  reloader = std::make_unique<reload::ReloadManager>();
  cached_primary = nullptr;
  auto primary_ptr = backpack->get_primary();
  auto secondary_ptr = backpack->get_secondary();
  auto frag_ptr = backpack->get_frag();
  if(secondary_ptr != nullptr){
    secondary = &secondary_ptr->stats;
    inventory.emplace_back((wpn::weapon_t)secondary_ptr->stats[WPN_TYPE]);
  }else{
    secondary = nullptr;
    inventory.emplace_back(wpn::weapon_t::WPN_P226);
  }
  if(primary_ptr != nullptr){
    m_debug("found primary_ptr");
    primary->feed(&primary_ptr->stats);
    inventory.emplace_back((wpn::weapon_t)primary_ptr->stats[WPN_TYPE]);
  }else{
    m_debug("feeding nullptr to primary");
    primary->feed(nullptr);
    inventory.emplace_back(wpn::weapon_t::WPN_MP5);
  }
  cached_primary = primary->weapon_stats();
  if(frag_ptr != nullptr){
    explosive_0 = &frag_ptr->stats;
    inventory.emplace_back((wpn::weapon_t)frag_ptr->stats[WPN_TYPE]);
  }else{
    explosive_0 = nullptr;
    inventory.emplace_back(wpn::weapon_t::WPN_FRAG);
  }
  equip_weapon(0);
  changing_weapon = 0;
	ready = true;
}
void Player::cycle_previous_weapon(){
  if(changing_weapon){
    m_debug("already changing weapon. rejecting cycle_previous_weapon");
    return;
  }
  if(weapon_index - 1 < 0){
    m_debug("start_equip_weapon inventory.size() - 1");
    start_equip_weapon(inventory.size() - 1);
    return;
  }
  m_debug("equipped_weapon -1");
  start_equip_weapon(weapon_index - 1);
}
void Player::cycle_next_weapon(){
  if(changing_weapon){
    m_debug("already changing weapon. rejecting cycle_next_weapon");
    return;
  }
  if(weapon_index + 1 == inventory.size()){
    m_debug("cycle_next_weapon 0 ");
    start_equip_weapon(0);
    return;
  }
    m_debug("cycle_next_weapon 1 ");
  start_equip_weapon(weapon_index + 1);
}
void Player::tick(){
  if(changing_weapon && has_target_at <= tick::get()){
    equip_weapon(target_equipped_weapon);
    target_equipped_weapon = -1;
    has_target_at = 0;
    changing_weapon = 0;
    return;
  }
  // TODO: when near loot, show open loot button
}
int Player::equip_weapon(int index){
  return equip_weapon(index,nullptr,nullptr);
}
int Player::equip_weapon(int index,weapon_stats_t* wpn,explosive_stats_t* exp){
  if(index < 0 || index >= inventory.size()){
    m_error("equipped_weapon has invalid index: " << index << ". Valid index is between zero and " << inventory.size() - 1);
    return -1;
  }
  weapon_index = index;
  m_debug("weapon_index: " << weapon_index);
  wpn::weapon_t wpn_type = (wpn::weapon_t)inventory[weapon_index];
  m_debug("wpn_type: '" << wpn_type << "'");//: '" << to_string(wpn_type) << "'");
  equipped_weapon = wpn_type;
  switch(wpn_type) {
    default:
      m_error("couldn't equip weapon of type: '" << wpn_type << "'");
      equipped_weapon_name = "???";
      return -2;
      break;
    case wpn::weapon_t::WPN_SPAS12:
      m_debug("WPN_SPAS12 recognized");
    case wpn::weapon_t::WPN_MP5:
      if(wpn){
        primary->feed(wpn);
      }else{
        if(cached_primary){
          primary->feed(cached_primary);
        }else{
          primary->feed(nullptr);
        }
      }
      cached_primary = primary->weapon_stats();
      equipped_weapon_name = primary->weapon_name();
      lambda_should_fire = [&]() -> const bool {
        static uint64_t last_tick = 0;
        if(last_tick + primary->stat(WPN_COOLDOWN_BETWEEN_SHOTS) <= tick::get()) {
          last_tick = tick::get();
          return true;
        }
        return false;
      };
      lambda_stat_index = [&](const uint8_t& _index) -> const uint32_t& {
        return primary->stat(_index);
      };
      lambda_dmg_lo = [&]() -> int {
        return primary->stat(WPN_DMG_LO);
      };
      lambda_dmg_hi = [&]() -> int {
        return primary->stat(WPN_DMG_HI);
      };

      clip_size = primary->stat(WPN_CLIP_SZ);
      ammo = primary->ammo_ptr();
      total_ammo = primary->total_ammo_ptr();
      reloader->update(&clip_size,ammo,total_ammo,primary->weapon_stats());
      wpn_stats = primary->weapon_stats();
      break;
    case wpn::weapon_t::WPN_P226:
    case wpn::weapon_t::WPN_GLOCK:
      m_debug("equipping pistol");
      if(!wpn){
        if(secondary){
          pistol->feed(*secondary);
          wpn_stats = secondary;
          equipped_weapon_name = weapon_name(wpn_stats);
          bcopy(secondary,&pistol->stats,sizeof(weapon_stats_t)); // FIXME: make stats a pointer just like mp5 class
        }else{
          pistol->feed(weapons::pistol::data::p226::stats);
          wpn_stats = pistol->weapon_stats();
          equipped_weapon_name = "p226";
        }
      }else{
        pistol->feed(*wpn);
        secondary = wpn_stats = wpn;
        equipped_weapon_name = weapon_name(wpn_stats);
      }
      lambda_should_fire = [&]() -> const bool {
        return pistol->should_fire();
      };
      lambda_stat_index = [&](const uint8_t& _index) -> const uint32_t& {
        return (*(wpn_stats))[_index];
      };
      lambda_dmg_lo = [&]() -> int {
        return (*(wpn_stats))[WPN_DMG_LO];
      };
      lambda_dmg_hi = [&]() -> int {
        return (*(wpn_stats))[WPN_DMG_HI];
      };

      clip_size = (*wpn_stats)[WPN_CLIP_SZ];
      ammo = &pistol->ammo;
      total_ammo = &pistol->total_ammo;
      reloader->update(&clip_size,ammo,total_ammo,wpn_stats);
      break;
    case wpn::weapon_t::WPN_FRAG:
      m_debug("equipping frag");
      equipped_weapon_name = "frag";
      lambda_should_fire = [&]() -> const bool {
        return true;
      };
      wpn_stats = nullptr;
      if(!exp){
        if(explosive_0){
          exp_stats = explosive_0;
        }else{
          exp_stats = frag->explosive_stats();
        }
      }else{
        explosive_0 = exp_stats = exp;
      }
      clip_size = 1;
      frag->total_ammo = 3;
      ammo = &frag->total_ammo;
      total_ammo = &frag->total_ammo;
      lambda_stat_index = [&](const uint8_t& _index) -> const uint32_t& {
        return (*(exp_stats))[_index];
      };
      lambda_dmg_lo = [&]() -> int {
        return (*(exp_stats))[EXP_DMG_LO];
      };
      lambda_dmg_hi = [&]() -> int {
        return (*(exp_stats))[EXP_DMG_HI];
      };
      reloader->update_frag(&clip_size,ammo,total_ammo,exp_stats);

      break;
  }
  return 0;
}
bool Player::weapon_should_fire() {
  if(changing_weapon){
    m_debug("changing weapon. weapon_should_fire is returning false");
    return false;
  }
  return lambda_should_fire();
}
uint32_t Player::weapon_stat(WPN index) {
  return lambda_stat_index(index);
}
weapon_stats_t* Player::weapon_stats() {
  return wpn_stats;
}
std::pair<int,int> Player::gun_damage() {
  int d = rand_between((*wpn_stats)[WPN_DMG_LO],(*wpn_stats)[WPN_DMG_HI]);
  int crit = 0;
  if(rng::chance(10)){
     crit = rand_between((*wpn_stats)[WPN_DMG_HI], (*wpn_stats)[WPN_DMG_HI] * 2);
    m_debug("crit: " << crit);
  }
  return {d,crit};
}
int Player::start_equip_weapon(int index){
  if(index < 0 || index >= inventory.size()){
    m_error("start_equip_weapon has invalid index: " << index);
    return -1;
  }
  if(changing_weapon && has_target_at > tick::get()){
    m_debug("rejecting -2: " << has_target_at);
    return -2;
  }
  weapon_index = index;
  wpn::weapon_t wpn_type = inventory[weapon_index];
  has_target_at = tick::get();
  switch(wpn_type){
    case wpn::weapon_t::WPN_SPAS12:
    case wpn::weapon_t::WPN_MP5:
      has_target_at += primary->stat(WPN_WIELD_TICKS);
      break;
    case wpn::weapon_t::WPN_GLOCK:
    case wpn::weapon_t::WPN_P226:
      has_target_at += (*pistol->weapon_stats())[WPN_WIELD_TICKS];
      break;
    default:
      has_target_at += 1500;
      break;
  }
  target_equipped_weapon = weapon_index;
  changing_weapon = true;
  return 0;
}
SDL_Texture* Player::initial_texture() {
  return this->self.bmp[0].texture;
}
void Player::calc() {
  cx =  self.rect.x + W / SCALE;
  cy =  self.rect.y + H / SCALE;
}
void Player::calc_outline() {
  calc();
  outline[0].x = cx;
  outline[0].y = cy;

  outline[1].x = cx - 20;
  outline[1].y = cy + 20;

  outline[2].x = cx - 20;
  outline[2].y = cy + 50;

  outline[3].x = cx + 20;
  outline[3].y = cy + 50;

  outline[4].x = cx + 20;
  outline[4].y = cy + 20;

  outline[5].x = cx;
  outline[5].y = cy;

  auto tmp_angle = angle;
  if(tmp_angle >= 247.5 && tmp_angle <= 292.5) {
    tmp_angle = 0;
  }
  tmp_angle = tmp_angle * PI / 180;
  float x,y;
  for(std::size_t i=0; i < OUTLINE_POINTS; i++) {
    x = outline[i].x;
    y = outline[i].y;
    outline[i].x = cx + ((x - cx) * cos(tmp_angle) - (y - cy) * sin(tmp_angle));
    outline[i].y = cy + ((x - cx) * sin(tmp_angle) + (y - cy) * cos(tmp_angle));
  }
}


namespace plr {
  static Player* p;
  Actor* self() {
    return &p->self;
  }
  Player* get() {
    return p;
  }
  uint16_t ammo() {
    // TODO: make these pointers
    switch(p->equipped_weapon){
      default:
        return 0;
      case wpn::weapon_t::WPN_FRAG:
        return p->frag->total_ammo;
      case wpn::weapon_t::WPN_MP5:
      case wpn::weapon_t::WPN_SPAS12:
        return p->primary->ammo;
      case wpn::weapon_t::WPN_P226:
      case wpn::weapon_t::WPN_GLOCK:
        return p->pistol->ammo;
    }
  }
  uint16_t total_ammo() {
    // TODO: make these pointers
    switch(p->equipped_weapon){
      default:
        return 0;
      case wpn::weapon_t::WPN_FRAG:
        return p->frag->total_ammo;
      case wpn::weapon_t::WPN_MP5:
      case wpn::weapon_t::WPN_SPAS12:
        return p->primary->total_ammo;
      case wpn::weapon_t::WPN_P226:
      case wpn::weapon_t::WPN_GLOCK:
        return p->pistol->total_ammo;
    }
  }
  void run(bool t) {
    p->running = t;
  }
  int& movement_amount() {
    if(p->running) {
      p->movement_amount = RUN_MOVEMENT_AMOUNT;
    } else {
      p->movement_amount = BASE_MOVEMENT_AMOUNT;
    }
    return p->movement_amount;
  }
  std::pair<int,int> gun_damage() {
    return p->gun_damage();
  }
  void start_gun() {
    if(p->changing_weapon){
      return;
    }
    if(p->holding_grenade_at == 0 && p->equipped_weapon == wpn::weapon_t::WPN_FRAG){
      if(!p->grenade_manager->done()){
        return;
      }
      if(*p->ammo == 0){
        return;
      }
      p->holding_grenade_at = tick::get();
      p->grenade_manager->set_grenade(p->frag->explosive_stats(),plr::cx(),plr::cy());
      p->grenade_manager->hold_grenade();
      return;
    }
    p->firing_weapon = true;
  }
  void stop_gun() {
    if(p->changing_weapon){
      return;
    }
    if(p->holding_grenade_at != 0 && p->equipped_weapon == wpn::weapon_t::WPN_FRAG){
      p->grenade_manager->toss_towards(cursor::mx(),cursor::my());
      p->holding_grenade_at = 0;
      p->consume_ammo();
      return;
    }

    p->firing_weapon = false;
  }
  uint32_t ms_registration() {
    return (*(p->weapon_stats()))[WPN_MS_REGISTRATION];
  }
  bool should_fire() {
    if(p->changing_weapon){
      m_debug("changing weapon. should_fire is returning false");
      return false;
    }
    return p->firing_weapon;
  }

  void draw_outline() {
    save_draw_color();
    set_draw_color("red");
    SDL_RenderDrawLinesF(ren,
        &p->outline[0],
        OUTLINE_POINTS
        );
    restore_draw_color();
  }
  void rotate_guy() {
    p->angle = coord::get_angle(p->cx,p->cy,cursor::mx(),cursor::my());
    if(draw_state::player::draw_guy()) {
      SDL_RenderCopyEx(
          ren,  //renderer
          p->self.bmp[0].texture,
          nullptr,// src rect
          &p->self.rect,
          p->angle, // angle
          nullptr,  // center
          SDL_FLIP_NONE // flip
          );
    }

    p->calc_outline();
  }
  void set_guy(Player* g) {
    p = g;
  }
  int& cx() {
    return p->cx;
  }
  int& cy() {
    return p->cy;
  }
  int& get_cx() {
    return p->cx;
  }
  int& get_cy() {
    return p->cy;
  }
  void calc() {
    p->calc();
  }
  SDL_Rect* get_rect() {
    return &p->self.rect;
  }
  void take_damage(weapon_stats_t * stats) {
    p->hp -= rand_between((*stats)[WPN_DMG_LO],(*stats)[WPN_DMG_HI]);
  }

  void redraw_guy() {
    if(draw_state::player::draw_guy()) {
      SDL_RenderCopyEx(
          ren,  //renderer
          p->self.bmp[0].texture,
          nullptr,// src rect
          &p->self.rect,
          p->angle, // angle
          nullptr,//&center,  // center
          SDL_FLIP_NONE// flip
          );
    }

#ifdef DRAW_OUTLINE
    draw_outline();
#endif
  }
#define DRAW_COLLISIONS
  void draw_collision_outline(SDL_Rect* _proposed_outline) {
    draw::draw_green();
    SDL_RenderDrawRect(ren,_proposed_outline);
    draw::restore_color();

  }

  void restore_collision_outline(SDL_Rect* _result) {
    draw::draw_red();
    SDL_RenderDrawRect(ren,_result);
    draw::restore_color();
  }
  void draw_player_rects() {
    save_draw_color();
    set_draw_color("green");
    SDL_RenderDrawRect(ren,&p->self.rect);
    set_draw_color("red");
    auto efm = get_effective_move_rect();
    SDL_RenderDrawRect(ren,efm);
    restore_draw_color();
  }
  void draw_reticle() {
    if(draw_state::reticle::draw_reticle()) {
      save_draw_color();
      set_draw_color("red");
      shapes::DrawCircle(p->cx,p->cy,51);
      restore_draw_color();
      uint8_t r,g,b,a;
      SDL_GetRenderDrawColor(ren,&r,&g,&b,&a);
      SDL_SetRenderDrawColor(ren,255,0,0,0);
      SDL_RenderDrawLine(ren,
          p->cx,
          p->cy,
          cursor::mx(),
          cursor::my());
      auto color = colors::green();
      SDL_SetRenderDrawColor(ren,color[0],color[1],color[2],0);
      SDL_RenderDrawLine(ren,
          p->cx,
          p->cy,
          top_right.x,
          top_right.y
          );
      /**
       * Draw a line between (cx,0)
       */
      /* Draw line up to north */
      draw::line(p->cx,p->cy,p->cx,0);
      // draw line right to east
      draw::line(p->cx,p->cy,win_width(),p->cy);
      // draw line down to south
      draw::line(p->cx,p->cy,p->cx,win_height());
      // draw line left to west
      draw::line(p->cx,p->cy,0,p->cy);

      draw_player_rects();
      SDL_SetRenderDrawColor(ren,r,g,b,a);
    }
  }
  int get_scale() {
    return ::SCALE ;
  }
  int get_width() {
    return ::W;
  }
  int get_height() {
    return ::H;
  }
  SDL_Rect* get_effective_rect() {
    static SDL_Rect r;
    r = p->self.rect;
    switch(dir::get_facing(p->angle)) {
      case Direction::SOUTH_EAST:
        /** Facing SOUTH EAST */
        r.y -= plr::get_height() / 1.3;
        r.w -= plr::get_width() / 2;
        r.h += plr::get_height() / 4;
        break;
      case Direction::EAST:
        /** FACING EAST */
        r.w -= plr::get_width() / 2;
        break;
      case Direction::NORTH_EAST:
        /** FACING NORTH EAST */
        r.w -= plr::get_width() / 2;
        r.h += plr::get_height() / 2;
        break;
      case Direction::NORTH:
        /** FACING NORTH */
        r.w -= plr::get_width() / 2;
        r.h += plr::get_height() / 2;
        r.x += plr::get_width() / 4;
        r.y += plr::get_height() / 4;
        break;
      case Direction::NORTH_WEST:
        /** FACING NORTH WEST */
        r.x += plr::get_width() / 2;
        r.y += plr::get_height() / 2;
        r.w -= plr::get_width() / 2;
        break;
      case Direction::WEST:
        /** FACING WEST */
        //r = p->self.rect;
        r.w -= plr::get_width() / 2;
        r.x += plr::get_width() / 2;
        break;
      case Direction::SOUTH_WEST:
        /** FACING SOUTH WEST */
        r.x += plr::get_width() / 2;
        r.y -= plr::get_height() / 2;
        r.w -= plr::get_width() / 2;
        break;
      default:
      case Direction::SOUTH:
        /** FACING SOUTH */
        r.x += plr::get_width() / 4;
        r.y -= plr::get_height() / 1.3;
        r.w -= plr::get_width() / 2;
        r.h += plr::get_height() / 2;
        break;
    }
    return &r;
  }
  SDL_Rect* get_effective_move_rect() {
    /**
     * FIXME: this is hacky, but _good enough_ to work
     */
    static SDL_Rect r;
    r = p->self.rect;
    r.y -= plr::get_height() / 2;
    r.w -= plr::get_width() / 2;
    r.w *= 2;
    r.h += plr::get_height() / 4;
    r.h *= 2;
    r.x += 10;
    r.w -= 20;
    r.h -= 10;
    return &r;
  }
  void update_reload_state(const reload::reload_phase_t& phase) {
  }
  void tick(){
    p->tick();
  }
};
