#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "npc-bomber.hpp"
#include "player.hpp"
#include "direction.hpp"
#include "npc/paths.hpp"
#include "font.hpp"
#include "colors.hpp"
#include "rng.hpp"
#include "sound/gunshot.hpp"
#include "sound/npc.hpp"
#include "events/death.hpp"
#include "damage/explosions.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[DEBUG]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";
#define m_error(A) std::cout << "[ERROR]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";

#define USE_PATH_TESTING_NORTH_EAST
extern bool can_move_direction(int direction,SDL_Rect* p,int adjustment);
extern std::pair<bool,uint8_t> check_can_move(SDL_Rect* p, int dir, int amount);
extern void calculateDestination(double x1, double y1, double angleDegrees, double distance, double& x2, double& y2);

namespace wall {
  extern std::vector<wall::Wall*> spawn_tiles;
};
namespace npc {
  extern std::size_t alive_counter;
  double calculateDistance(double x1, double y1, double x2, double y2) {
    // Using the distance formula: sqrt((x2 - x1)^2 + (y2 - y1)^2)
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
  } 
  void calculateDestination(double x1, double y1, double angleDegrees, double distance, double& x2, double& y2) {
    // Convert angle from degrees to radians
    double angleRadians = angleDegrees * M_PI / 180.0;

    // Calculate the destination coordinates
    x2 = x1 + distance * cos(angleRadians);
    y2 = y1 + distance * sin(angleRadians);
  }
  namespace bomber::data {
    static SDL_mutex * bomber_list_mutex = SDL_CreateMutex();
    std::forward_list<Bomber> bomber_list;
    int bomber_mode = 0;
    static bool halt_bomber = false;
    static constexpr std::size_t SLASH_WIDTH = 80;
    static constexpr std::size_t SLASH_HEIGHT = 53;
    static constexpr std::size_t SLASH_MOVEMENT = 20;
    static constexpr const char* BMP = "../assets/bomber-0.bmp";
    static constexpr const char* HURT_BMP = "../assets/bomber-hurt-%d.bmp";
    static constexpr std::size_t HURT_BMP_COUNT = 3;
    static constexpr const char* DEAD_BMP = "../assets/bomber-dead-%d.bmp";
    static constexpr std::size_t DEAD_BMP_COUNT = 1;
    //static constexpr const char* DETONATED_BMP = "../assets/spet-detonated-arm-%d.bmp";
    //static constexpr std::size_t DETONATED_BMP_COUNT= 6;
    static constexpr int CENTER_X_OFFSET = 110;
    static constexpr uint16_t COOLDOWN_BETWEEN_SHOTS = 810;
    static constexpr float AIMING_RANGE_MULTIPLIER = 1.604;
    static constexpr uint16_t STUNNED_TICKS = 300;
    static constexpr const char* SPLATTERED_BMP = "../assets/spet-dead-splattered-0.bmp";
    static std::unique_ptr<Actor> attack_actor = nullptr;
    static std::unique_ptr<Actor> slash_actor = nullptr;

    static constexpr int BOMBER_MAX_HP = 100;
    static constexpr int BOMBER_LOW_HP = 75;
    static constexpr int BOMBER_RANDOM_LO = 10;
    static constexpr int BOMBER_RANDOM_HI = 25;
    static constexpr int SEE_DISTANCE = 20;
    static constexpr weapon_stats_t MACHETE = {
      0,//WPN_FLAGS = 0,
      wpn::weapon_t::WPN_MACHETE,//WPN_TYPE,
      50,//WPN_DMG_LO,
      120,//WPN_DMG_HI,
      3,//WPN_BURST_DLY,
      3,//WPN_PIXELS_PT,
      3,//WPN_CLIP_SZ,
      3,//WPN_AMMO_MX,
      3,//WPN_RELOAD_TM,
      40,//WPN_COOLDOWN_BETWEEN_SHOTS,
      0,//WPN_MS_REGISTRATION,
      0,//WPN_MAG_EJECT_TICKS,
      0,//WPN_PULL_REPLACEMENT_MAG_TICKS,
      0,//WPN_LOADING_MAG_TICKS,
      0,//WPN_SLIDE_PULL_TICKS,
      3,//WPN_WIELD_TICKS,
      100,//WPN_ACCURACY,
      0,//WPN_ACCURACY_DEVIATION_START,
      0//WPN_ACCURACY_DEVIATION_END,
    };

    static std::vector<Actor*> dead_list;

  };

  void Bomber::report(){
    bool csp = can_see_player();
    if(csp){
      std::cout << "I can see player\n";
    }else{
      std::cout << "I cannot see player\n";
    }
    if(!wandering_mode){
      std::cout << "not wandering\n";
    }else{
      std::cout << "wandering\n";
    }
    if(blocked){
      std::cout << "Am blocked. trying brute force:\n";
      wander_direction = -1;
      for(const auto dir : {NORTH_EAST,NORTH_WEST,SOUTH_EAST,SOUTH_WEST,EAST,WEST,NORTH,SOUTH}){
        bool ok = false;
        switch(dir){
          case EAST:
            ok = move_east();
            break;
          case WEST:
            ok = move_west();
            break;
          case NORTH:
            ok = move_north();
            break;
          case NORTH_EAST:
            ok = move_north_east();
            break;
          case NORTH_WEST:
            ok = move_north_west();
            break;
          case SOUTH:
            ok = move_south();
            break;
          case SOUTH_WEST:
            ok = move_south_west();
            break;
          case SOUTH_EAST:
            ok = move_south_east();
            break;
          default:
            break;
        }
        if(ok){
          wander_direction = dir;
          std::cout << "found good direction: " << wander_direction << "\n";
          break;
        }
      }
      if(wander_direction == -1){
        std::cout << "couldn't find a good direction!\n";
      }
    }else{
      std::cout << "I am _NOT_ blocked. wander_direction: " << wander_direction << "\n";
    }
  }
  uint16_t Bomber::cooldown_between_shots() {
    using namespace npc::bomber::data;
    return COOLDOWN_BETWEEN_SHOTS;
  }
  bool Bomber::can_slash_again() {
    using namespace npc::bomber::data;
    return m_last_slash_tick + cooldown_between_shots() <= tick::get();
  }
  void Bomber::slash_at_player() {
    using namespace npc::bomber::data;
    if(m_last_slash_tick > tick::get()){
      return;
    }
    // TODO: telegraph slash
    m_last_slash_tick = tick::get() + rand_between(1980,2430);
    calc();
    SDL_Rect r;
    double x1,y1,angleDegrees,distance,x2,y2;
    x1 = self.rect.x;
    y1 = self.rect.y;
    angleDegrees = angle;
    distance = 90;
    calculateDestination(x1, y1, angleDegrees, distance, x2, y2);
#ifdef DRAW_BOMBER_PRE_HIT_LINE
    draw::line(x1,y1,x2,y2);
#endif
    r.x = x2;
    r.y = y2;
    r.w = 110;
    r.h = 60;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    int angle = coord::get_angle(self.rect.x,self.rect.y,x2,y2);
    SDL_RenderCopyEx(
        ren,  //renderer
        slash_actor->bmp[0].texture,
        nullptr,// src rect
        &r,
        angle, // angle
        nullptr,  // center
        flip // flip
        );
    SDL_Rect result;
    if(SDL_IntersectRect(&r,&plr::get()->self.rect,&result)){
      plr::take_damage(machete.stats);
    }
  }
  void Bomber::die(){
    m_debug("DIED");
    sound::npc::play_death_sound(Bomber::TYPE_ID);
    events::death::dispatch(Bomber::TYPE_ID,id,cx,cy);
  }
  bool Bomber::dead(){
    return hp <= 0;
  }
  void Bomber::corpse_hit(){
    sound::npc::play_corpse_sound(Bomber::TYPE_ID,hp);
  }
  void Bomber::take_damage(int damage) {
    using namespace npc::bomber::data;
    if(dead()){
      //corpse_hit();
      return;
    }

    hp -= damage;
    if(hp <= 0){
      dismembered = false;
      die();
      hp = 0;
      self.bmp[0] = dead_actor.self.bmp[rand_between(0,dead_actor.self.bmp.size()-1)];
      dead_list.emplace_back(&self);
      return;
    }
    sound::npc::play_npc_pain(Bomber::TYPE_ID);
    self.bmp[0] = *next_state();
    m_stunned_until = STUNNED_TICKS + rand_between(200,500) + tick::get();
  }
  void Bomber::move_to(const int32_t& x,const int32_t& y) {
    self.rect.x = x;
    self.rect.y = y;
  }
  bool Bomber::can_see_player() {
    vpair_t s{self.rect.x,self.rect.y};
    auto tile = paths::get_tile(s);
    vpair_t p{plr::self()->rect.x,plr::self()->rect.y};
    auto ptile = paths::get_tile(p);
    if(!tile || !ptile){
      return false;
    }
    return paths::has_line_of_sight(tile,ptile);
  }
  void Bomber::next_waypoint(){
    if(can_see_player()){
      next_path.x = plr::get()->cx;
      next_path.y = plr::get()->cy;
      return;
    }
    next_path_candidate = path_finder->next_point();
    if(next_path_candidate){
      next_path = *next_path_candidate;
    }
  }
  void Bomber::walk_to_next_path() {
    if(can_see_player()){
      next_path.x = plr::get()->cx;
      next_path.y = plr::get()->cy;
      rush_at_player();
      return;
    }
    if(next_path.x < cx) {
      move_west();
    } else if(next_path.x > cx) {
      move_east();
    }
    if(next_path.y > cy) {
      move_south();
    } else if(next_path.y < cy) {
      move_north();
    }
    if(!can_see_player() && calculateDistance(next_path.x,next_path.y,cx,cy) < CELL_WIDTH / 4){
      next_waypoint();
    }
    calc();
  }
  void Bomber::rush_at_player(){
    m_debug("rush_at_player");
    trajectory.clear();
    trajectory_index = 0;
    velocity = 0;

    // Calculate the differences in x and y coordinates
    double x2 = plr::get()->cx;
    double y2 = plr::get()->cy;
    double x1 = cx;
    double y1 = cy;
    double deltaX = x2 - x1;
    double deltaY = y2 - y1;

    // Use the arctangent function (atan2) to calculate the angle
    double angleInRadians = atan2(deltaY, deltaX);

    // Convert radians to degrees
    double angleInDegrees = angleInRadians * 180.0 / M_PI;
    draw::line(x1,y1,x2,y2);
    double save_x,save_y;
    int i = 0;
    int ctr = 0;
    while(ctr++ < 15){
      ++i;
      calculateDestination(x1,y1,angleInDegrees,i * 20 ,save_x,save_y);

      auto distance = calculateDistance(save_x, save_y, plr::get()->cx,plr::get()->cy);
      trajectory.emplace_back(save_x,save_y);
    }
    velocity = i + 1;
  }
  void Bomber::start_wandering(){
    wandering_mode = true;
    wander_started_tick = tick::get();
    wander_tick = tick::get() + rand_between(1,5) * 1000;
    wander_direction = npc::paths::get_direction_toward_player(&self.rect);
  }
  void Bomber::perform_ai() {
    if(m_stunned_until > tick::get()) {
      return;
    }
    calc();
    if(!can_see_player() && perform_ai_tick <= tick::get()){
      update_check();
      perform_ai_tick = tick::get() + 20000;
    }
    if(!can_see_player()){
      walk_to_next_path();
      next_waypoint();
      return;
    }
    if(can_see_player() && last_rush_tick <= tick::get()){
      next_path.x = plr::get()->cx;
      next_path.y = plr::get()->cy;
      walk_to_next_path();
      rush_at_player();
      calc();
      last_rush_tick = tick::get() + rand_between(800,1200);
    }
    auto distance = calculateDistance(cx, cy, plr::get()->cx,plr::get()->cy);
    if(distance < 190){
      SDL_Point p{cx,cy};
      damage::explosions::detonate_at(&p, //SDL_Point* p,
                                      rand_between(140,260),    //const uint16_t& radius,
                                      rand_between(80,130),    //const uint16_t& damage,
                                      0);    //const uint8_t& type);
      hp = 0;
    }
  }
  bool Bomber::within_aiming_range(){
    using namespace npc::bomber::data;
    return npc::paths::distance(&self,plr::self()) < SEE_DISTANCE;
  }
  SDL_Texture* Bomber::initial_texture() {
    return self.bmp[0].texture;
  }
  void Bomber::calc() {
    plr::calc();
    cx = self.rect.x + self.rect.w / 2;
    cy = self.rect.y + self.rect.h / 2;
    angle = coord::get_angle(cx,cy,plr::get_cx(),plr::get_cy());
  }
  void Bomber::tick() {

    if(velocity > 0 && trajectory.size() > trajectory_index){
      SDL_Rect r{trajectory[trajectory_index].x,trajectory[trajectory_index].y,80,80};
      if(wall::is_blocked(&r)){
        velocity = 0;
      }else{
        self.rect.x = trajectory[trajectory_index].x;
        self.rect.y = trajectory[trajectory_index].y;
        --velocity;
        ++trajectory_index;
      }
    }
    perform_ai();

  }
  Asset* Bomber::next_state() {
    //if(dismembered){
    //  return &splattered_actor->bmp[0];
    //}
    if(hp <= 0) {
      return &dead_actor.self.bmp[0];
    }
    return states[0];
  }
  bool Bomber::move_south() {
    std::pair<bool,uint8_t> p = check_can_move(&self.rect,//SDL_Rect* p, 
        SOUTH, //int dir,
        movement_amount);//int amount);
    if(p.first){
      self.rect.y += movement_amount;
      blocked = false;
      return true;
    }
    blocked = true;
    return false;
  }
  bool Bomber::move_north() {
    std::pair<bool,uint8_t> p = check_can_move(&self.rect,//SDL_Rect* p, 
        NORTH, //int dir,
        movement_amount);//int amount);
    if(p.first){
      self.rect.y -= movement_amount;
      blocked = false;
      return true;
    }
    blocked = true;
    return false;
  }

  bool Bomber::move_west() {
    std::pair<bool,uint8_t> p = check_can_move(&self.rect,//SDL_Rect* p, 
        WEST, //int dir,
        movement_amount);//int amount);
    if(p.first){
      self.rect.x -= movement_amount;
      blocked = false;
      return true;
    }
    blocked = true;
    return false;
  }
  bool Bomber::move_east() {
    std::pair<bool,uint8_t> p = check_can_move(&self.rect,//SDL_Rect* p, 
        EAST, //int dir,
        movement_amount);//int amount);
    if(p.first){
      self.rect.x += movement_amount;
      blocked = false;
      return true;
    }
    blocked = true;
    return false;
  }
  bool Bomber::move_south_east() {
    std::pair<bool,uint8_t> p = check_can_move(&self.rect,//SDL_Rect* p, 
        SOUTH_EAST, //int dir,
        movement_amount);//int amount);
    if(p.first && p.second == SOUTH_EAST){
      self.rect.x += movement_amount;
      self.rect.y += movement_amount;
      blocked = false;
      return true;
    }
    blocked = true;
    return false;
  }
  bool Bomber::move_south_west() {
    std::pair<bool,uint8_t> p = check_can_move(&self.rect,//SDL_Rect* p, 
        SOUTH_WEST, //int dir,
        movement_amount);//int amount);
    if(p.first && p.second == SOUTH_WEST){
      self.rect.x -= movement_amount;
      self.rect.y += movement_amount;
      blocked = false;
      return true;
    }
    blocked = true;
    return false;
  }
  bool Bomber::move_north_east() {
    std::pair<bool,uint8_t> p = check_can_move(&self.rect,//SDL_Rect* p, 
        NORTH_EAST, //int dir,
        movement_amount);//int amount);
    if(p.first && p.second == NORTH_EAST){
      self.rect.x += movement_amount;
      self.rect.y -= movement_amount;
      blocked = false;
      return true;
    }
    blocked = true;
    return false;
  }
  bool Bomber::move_north_west() {
    std::pair<bool,uint8_t> p = check_can_move(&self.rect,//SDL_Rect* p, 
        NORTH_WEST, //int dir,
        movement_amount);//int amount);
    if(p.first && p.second == NORTH_WEST){
      self.rect.x -= movement_amount;
      self.rect.y -= movement_amount;
      blocked = false;
      return true;
    }
    blocked = true;
    return false;
  }
  int Bomber::center_x_offset() {
    using namespace npc::bomber::data;
    return CENTER_X_OFFSET;
  }
  const bool Bomber::is_dead() const {
    return hp <= 0;
  }
  uint32_t Bomber::weapon_stat(WPN index) {
    return (*(machete.stats))[index];
  }
  weapon_stats_t* Bomber::weapon_stats() {
    return machete.stats;
  }

  Bomber::Bomber() {
    using namespace npc::bomber::data;
    velocity = 0;
    trajectory_index = 0;
    trajectory.clear();
    blocked = false;
    path_finder = std::make_unique<npc::paths::PathFinder>(SLASH_MOVEMENT,&self,plr::self());
    path_finder->set_direct_line_only(true);
    path = &path_finder->chosen_path->path;
    wandering_mode = false;
    last_rush_tick = last_vocal = perform_ai_tick = tick::get() + rand_between(200,6200);
    dismembered = false;
    ready = false;
    next_path = {0,0};
    rush_charge = 3;
  }
  Bomber::Bomber(const int32_t& _x,
      const int32_t& _y,
      const int& _ma,
      const npc_id_t& _id) {
    rush_charge = 3;
    using namespace npc::bomber::data;
    velocity = 0;
    trajectory_index = 0;
    trajectory.clear();
    blocked = false;
    path_finder = std::make_unique<npc::paths::PathFinder>(SLASH_MOVEMENT,&self,plr::self());
    path_finder->set_direct_line_only(true);
    path = &path_finder->chosen_path->path;
    wandering_mode = false;
    dismembered = false;
    self.rect.x = _x;
    self.rect.y = _y;
    self.rect.w = SLASH_WIDTH;
    self.rect.h = SLASH_HEIGHT;
    movement_amount = _ma;
    self.load_bmp_asset(BMP);
    hurt_actor.self.load_bmp_assets(HURT_BMP,HURT_BMP_COUNT);
    dead_actor.self.load_bmp_assets(DEAD_BMP,DEAD_BMP_COUNT);
    hp = BOMBER_LOW_HP;
    max_hp = BOMBER_MAX_HP;
    ready = true;

    state_index = 0;
    for(int i=0; i < hurt_actor.self.bmp.size(); ++i) {
      states.emplace_back(&hurt_actor.self.bmp[i]);
    }
    id = _id;
    calc();
    m_last_slash_tick = 0;
    m_stunned_until = 0;
    next_path = {self.rect.x,self.rect.y};
    move_to(_x,_y);
    last_rush_tick = last_vocal = perform_ai_tick = tick::get() + rand_between(200,6200);
  }
  void Bomber::update_check() {
    if(blocked){
      path_finder->update(&self,plr::self());
    }
    next_waypoint();
  }
  Bomber::~Bomber(){
    states.clear();
    path_finder = nullptr;
  }
  void Bomber::cleanup(){
    states.clear();
    path_finder = nullptr;
  }
  void Bomber::take_explosive_damage(int damage,SDL_Rect* source_explosion,int blast_radius, int on_death,SDL_Rect* source_rect){
    using namespace npc::bomber::data;
    if(dead()){
      return;
    }
    hp -= damage;
    if (hp <= 0){
      die();
      dismembered = false;
    }
  }
  bool Bomber::is_slashing() const{
    return velocity > 0 && trajectory_index < trajectory.size();
  }
  namespace bomber {
    void spawn_bomber_at(const int32_t& x,const int32_t& y){
      bomber::data::bomber_list.emplace_front(x,y,bomber::data::SLASH_MOVEMENT,npc_id::next());
      world->npcs.push_front(&bomber::data::bomber_list.front().self);
    }
    void spawn_bomber(const std::size_t& count) {
      if(bomber::data::halt_bomber){
        return ;
      }
      m_debug("spawn_bomber: " << count);
      std::size_t ctr = 0;
      std::map<wall::Wall*,size_t> counts;
      for(const auto& w : wall::spawn_tiles){
        counts[w] = 0;
      }
      std::vector<wall::Wall*> shuffled = rng::shuffle_container(wall::spawn_tiles);
      m_debug("shuffled.size(): " << shuffled.size());
      for(ctr=0; ctr < count;){
        for(const auto& w : shuffled){
          spawn_bomber_at(w->rect.x,w->rect.y);
          if(++ctr >= count){
            break;
          }
        }
      }
    }
    void take_explosive_damage(Actor* a, int damage,SDL_Rect* source_explosion,int blast_radius, int on_death,SDL_Rect* src_rect){
      using namespace npc::bomber::data;
      for(auto& s : bomber_list) {
        if(&s.self == a) {
          s.take_explosive_damage(damage,source_explosion,blast_radius,on_death,src_rect);
        }
      }
    }
    void program_exit(){
      using namespace npc::bomber::data;
      halt_bomber = true;
      for(auto& s : bomber_list){
        s.cleanup();
      }
      bomber_list.clear();
      dead_list.clear();
    }
    bool is_dead(Actor* a) {
      using namespace npc::bomber::data;
      return std::find(dead_list.cbegin(), dead_list.cend(), a) != dead_list.cend();
    }
    void cleanup_corpses() {
      using namespace npc::bomber::data;
      std::vector<Actor*> corpse_actors;
      std::size_t size = 0;
      for(auto& sp : bomber_list) {
        ++size;
        if(sp.is_dead()) {
          corpse_actors.emplace_back(&sp.self);
        }
      }
      dead_list.clear();
      if(corpse_actors.size()) {
        cleanup_dead_npcs(corpse_actors);
      }
      m_debug("cleanup_corpses bomber_list size: " << size);
      bomber_list.remove_if([&](const auto& sp) -> bool {
          return sp.is_dead();
          });
      size = 0;
      for(auto& sp : bomber_list) {
        ++size;
      }
      m_debug("AFTER cleanup_corpses bomber_list size: " << size);
    }
    void take_damage(Actor* a,int dmg) {
      using namespace npc::bomber::data;
      if(halt_bomber){
        return;
      }
      for(auto& s : bomber::data::bomber_list) {
        if(&s.self == a) {
          s.take_damage(dmg);
        }
      }
    }
    void bomber_movement(uint8_t dir,int adjustment) {
      using namespace npc::bomber::data;
      if(halt_bomber){
        return;
      }
      auto adj = abs(adjustment);
      for(auto& s : bomber::data::bomber_list) {
        if(s.is_dead()) {
          continue;
        }
        switch(Direction(dir)) {
          case NORTH_WEST:
            s.self.rect.x += adj;
            s.self.rect.y += adj;
            if(s.velocity){
              for(int i=0; i < s.trajectory.size();i++){
                s.trajectory[i].x += adj;
                s.trajectory[i].y += adj;
              }
            }
            break;
          case NORTH_EAST:
            s.self.rect.x -= adj;
            s.self.rect.y += adj;
            if(s.velocity){
              for(int i=0; i < s.trajectory.size();i++){
                s.trajectory[i].x -= adj;
                s.trajectory[i].y += adj;
              }
            }
            break;
          case SOUTH_EAST:
            s.self.rect.x -= adj;
            s.self.rect.y -= adj;
            if(s.velocity){
              for(int i=0; i < s.trajectory.size();i++){
                s.trajectory[i].x -= adj;
                s.trajectory[i].y -= adj;
              }
            }
            break;
          case SOUTH_WEST:
            s.self.rect.x += adj;
            s.self.rect.y -= adj;
            if(s.velocity){
              for(int i=0; i < s.trajectory.size();i++){
                s.trajectory[i].x += adj;
                s.trajectory[i].y -= adj;
              }
            }
            break;
          case WEST:
            s.self.rect.x += adj;
            if(s.velocity){
              for(int i=0; i < s.trajectory.size();i++){
                s.trajectory[i].x += adj;
              }
            }
            break;
          case EAST:
            s.self.rect.x -= adj;
            if(s.velocity){
              for(int i=0; i < s.trajectory.size();i++){
                s.trajectory[i].x -= adj;
              }
            }
            break;
          case SOUTH:
            s.self.rect.y -= adj;
            if(s.velocity){
              for(int i=0; i < s.trajectory.size();i++){
                s.trajectory[i].y -= adj;
              }
            }
            break;
          case NORTH:
            s.self.rect.y += adj;
            if(s.velocity){
              for(int i=0; i < s.trajectory.size();i++){
                s.trajectory[i].y += adj;
              }
            }
            break;
        }
        //s.calc();
      }
    }
    void tick() {
      using namespace npc::bomber::data;
      if(halt_bomber){
        return;
      }
      size_t ctr=0;
      for(auto& s : bomber::data::bomber_list) {
        if(s.is_dead()) {
          continue;
        } else {
#ifdef DRAW_PLR_TO_BOMBER_LINE
          draw::line(s.self.rect.x, s.self.rect.y,plr::get()->self.rect.x,plr::get()->self.rect.y);
#endif
          s.tick();
          ++alive_counter;
        }
        auto texture = s.self.bmp[0].texture;
        if(s.is_slashing()){
          texture = attack_actor->bmp[0].texture;
        }

        SDL_RenderCopyEx(
            ren,  //renderer
            texture,
            nullptr,// src rect
            &s.self.rect,
            s.angle, // angle
            nullptr,  // center
            SDL_FLIP_NONE // flip
            );
      }
    }
    void init() {
      using namespace npc::bomber::data;
      if(halt_bomber){
        return ;
      }
      attack_actor = std::make_unique<Actor>();
      attack_actor->load_bmp_asset("../assets/bomber-slash-0.bmp");
      slash_actor = std::make_unique<Actor>();
      slash_actor->load_bmp_assets("../assets/slash-%d.bmp",2);
#ifdef TEST_NPC_BOMBERS
      auto x = world->start_tile_x();
      auto y = world->start_tile_y();
      for(int i=0; i < 1;i++){
        npc::bomber::spawn_bomber_at(x + (i * 20),y);
      }
#endif
    }
  };
};

#undef m_debug
