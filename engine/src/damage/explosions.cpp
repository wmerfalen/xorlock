#include <SDL2/SDL.h>
#include <iostream>
#include <forward_list>
#include "explosions.hpp"
#include "../player.hpp"
#include "../direction.hpp"
#include "../npc/paths.hpp"
#include "../font.hpp"
#include "../colors.hpp"
#include "../rng.hpp"
#include "../sound/gunshot.hpp"
#include "../sound/npc.hpp"
#include "../viewport.hpp"
#include "../constants.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[DAMAGE][explosion][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[DAMAGE][explosion][ERROR]: " << A << "\n";

namespace damage::explosions {
  static bool halt_explosions = false;
  static constexpr std::size_t explosion_WIDTH = 375;
  static constexpr std::size_t explosion_HEIGHT = 260;
  static int mode = 0;
  static constexpr std::size_t MAX_EXPLOSIONS_LIST_SIZE = 40;
  static std::set<explosion*> active_explosion_pointers;
  static std::array<std::unique_ptr<explosion>,MAX_EXPLOSIONS_LIST_SIZE> ptr_memory_pool;
  static std::size_t EXPLOSION_DIR_START = 0;
  static std::size_t EXPLOSION_DIR_STOP = 3;

  static constexpr std::size_t top_level_dir_first = 0;
  static constexpr const char* top_level_dir_pattern = "../assets/explosion-::XX::/%d.bmp";
  static constexpr const char* top_level_dp_replace = "::XX::";
  static constexpr std::pair<int,int> airstrike_radio_bounds ={0,2};
  static constexpr std::string_view airstrike_dir = "../assets/sound/airstrike/";
  static constexpr std::string_view airstrike_radio_pattern = "airstrike-radio-%d.wav";
  static constexpr size_t EXPLOSION_MAX = 4;
  static constexpr size_t AMBIENCE_MAX = 2;
  static std::array<std::pair<std::string_view,Mix_Chunk*>,EXPLOSION_MAX> explosion_wav_files = {
    std::make_pair<>("explosion-0",nullptr),
    std::make_pair<>("explosion-1",nullptr),
    std::make_pair<>("explosion-3",nullptr),
    std::make_pair<>("explosion-5",nullptr),
  };
  static std::array<std::pair<std::string_view,Mix_Chunk*>,AMBIENCE_MAX> ambience_wav_files = {
    std::make_pair<>("explosion-ambience-0",nullptr),
    std::make_pair<>("explosion-ambience-2",nullptr),
  };

  static SDL_mutex* active_explosion_mutex = SDL_CreateMutex();
  static SDL_mutex* ptr_mem_mutex = SDL_CreateMutex();

  void move_map(int dir, int amount){
    if(halt_explosions){
      return;
    }
    LOCK_MUTEX(active_explosion_mutex);
    for(auto& exp : active_explosion_pointers){
      if(exp->done){
        continue;
      }
      switch(dir) {
        case NORTH_EAST:
          exp->self.rect.y += amount;
          exp->self.rect.x -= amount;
          break;
        case NORTH_WEST:
          exp->self.rect.y += amount;
          exp->self.rect.x += amount;
          break;
        case NORTH:
          exp->self.rect.y += amount;
          break;
        case SOUTH_EAST:
          exp->self.rect.y -= amount;
          exp->self.rect.x -= amount;
          break;
        case SOUTH_WEST:
          exp->self.rect.y -= amount;
          exp->self.rect.x += amount;
          break;
        case SOUTH:
          exp->self.rect.y -= amount;
          break;
        case WEST:
          exp->self.rect.x += amount;
          break;
        case EAST:
          exp->self.rect.x -= amount;
          break;
        default:
          break;
      }
    }
    UNLOCK_MUTEX(active_explosion_mutex);
  }
  void draw_target(SDL_Point p){
    if(halt_explosions){
      return;
    }
    SDL_Rect r;
    r.x = p.x;
    r.y = p.y;
    r.w = 50;
    r.h = 50;
    draw::blatant_rect(&r);
  }

  int initial_x = 0;
  int initial_y = 0;
  std::vector<std::pair<int,int>> target_offsets = {
    {0,0},
    {125,-125},
    {400,-125},
    {125,125},
    {400,125},
  };
  static inline void cast_rays(){
    if(halt_explosions){
      return;
    }
    const int& x = plr::cx();
    const int& y = plr::cy();
    for(const auto& p : target_offsets){
      draw_target({x + p.first, y + p.second});
    }
  }
  static inline std::vector<SDL_Point> save_bomb_targets(){
    if(halt_explosions){
      return {};
    }
    const int& x = plr::cx();
    const int& y = plr::cy();
    std::vector<SDL_Point> list;
    for(const auto& p : target_offsets){
      list.emplace_back(x + p.first,y + p.second);
    }
    return list;
  }
  void init() {
    m_debug("init entry");
    for(size_t i=0; i < MAX_EXPLOSIONS_LIST_SIZE;i++){
      ptr_memory_pool[i] = nullptr;
    }
  }
  uint64_t next_space_bar_accepted_at = 0;
  void space_bar_pressed(){
    if(halt_explosions){
      return;
    }
    if(next_space_bar_accepted_at > tick::get()){
      return;
    }
    next_space_bar_accepted_at = tick::get() + 5000;
  }
  void tick() {
    if(halt_explosions){
      return;
    }
    static bool initial_set = false;
    if(!initial_set){
      initial_x = plr::cx();
      initial_y = plr::cy();
      initial_set = true;
    }

    LOCK_MUTEX(active_explosion_mutex);
    for(auto& ptr : active_explosion_pointers){
      if(ptr->done){
        continue;
      }
      ptr->tick();
    }
    UNLOCK_MUTEX(active_explosion_mutex);
  }
  SDL_Texture* explosion::initial_texture() {
    m_debug("returning initial_texture: " << self.bmp.size());
    return self.bmp[0].texture;
  }
  explosion::explosion(uint8_t directory_id,SDL_Point* p) : type(directory_id), angle(0), explosive_damage(rand_between(500,800)), radius(rand_between(100,450)), x(p->x),y(p->y),done(false) {
    if(halt_explosions){
      return;
    }
    std::string path = "";
    for(size_t i=0; i < strlen(top_level_dir_pattern);i++){
      if(0 == strncmp(&top_level_dir_pattern[i],top_level_dp_replace,strlen(top_level_dp_replace))){
        path += std::to_string(directory_id);
        i += strlen(top_level_dp_replace) - 1;
        continue;
      }
      path += top_level_dir_pattern[i];
    }
    m_debug("path: '" << path << "'");
    self.rect.x = x - radius;
    self.rect.y = y - radius;
    self.rect.w = radius * 2;
    self.rect.h = radius * 2;
    self.load_bmp_assets(path.c_str(),4,1);
    start_tick = tick::get();
  }
  void explosion::initialize_with(uint8_t directory_id,SDL_Point* p) {
    if(halt_explosions){
      return;
    }
    angle = 0;
    explosive_damage = rand_between(500,800);
    radius = rand_between(100,450); 
    x = p->x;
    y = p->y;
    done = false;
    if(type != directory_id){
      std::string path = "";
      for(size_t i=0; i < strlen(top_level_dir_pattern);i++){
        if(0 == strncmp(&top_level_dir_pattern[i],top_level_dp_replace,strlen(top_level_dp_replace))){
          path += std::to_string(directory_id);
          i += strlen(top_level_dp_replace) - 1;
          continue;
        }
        path += top_level_dir_pattern[i];
      }
      m_debug("path: '" << path << "'");
      self.free_existing();
      self.load_bmp_assets(path.c_str(),4,1);
      type = directory_id;
    }
    self.rect.x = x - radius;
    self.rect.y = y - radius;
    self.rect.w = radius * 2;
    self.rect.h = radius * 2;
    start_tick = tick::get();
  }
  void explosion::tick() {
    if(halt_explosions){
      return;
    }
    if(start_tick + 1500 < tick::get()){
      ++phase;
      start_tick = tick::get();
    }
    if(phase >= self.bmp.size()){
      done = true;
      return;
    }
    if(!self.bmp[phase].texture){
      m_error("TEXTURE FOR PHASE " << phase << " is nullptr!!!");
      return;
    }
    SDL_RenderCopyEx(
        ren,  //renderer
        self.bmp[phase].texture,
        nullptr,// src rect
        &self.rect,
        angle, // angle
        nullptr,  // center
        SDL_FLIP_NONE // flip
        );
  }
  Asset* explosion::next_state() {
    return states[0];
  }

  void detonate_at(SDL_Point* p,int damage,int type){
    if(halt_explosions){
      m_debug("halt_explosions preventing detonate_at");
      return;
    }
    m_debug("DETONATE_AT: " << p->x << "," << p->y << " [" << damage << "](" << type << ")");

    int empty_index = -1;
    LOCK_MUTEX(ptr_mem_mutex);
    for(size_t i=(type * 10 - 1); i < (type * 10 - 1) + 10; i++){
      if(ptr_memory_pool[i] && ptr_memory_pool[i]->done && ptr_memory_pool[i]->type == type){
          m_debug("found ->done with type: " << i);
          ptr_memory_pool[i]->initialize_with(type,p);
          ptr_memory_pool[i]->trigger_explosion();
          UNLOCK_MUTEX(ptr_mem_mutex);
          return;
      }
      if(ptr_memory_pool[i] && ptr_memory_pool[i]->done){
        m_debug("found ->done: " << i);
        ptr_memory_pool[i]->initialize_with(type,p);
        ptr_memory_pool[i]->trigger_explosion();
        UNLOCK_MUTEX(ptr_mem_mutex);
        return;
      }
      if(ptr_memory_pool[i] == nullptr && empty_index == -1){
        empty_index = i;
      }
    }
    if(empty_index != -1){
      m_debug("found empty ptr: " << empty_index);
      ptr_memory_pool[empty_index] = std::make_unique<explosion>(type,p);
      ptr_memory_pool[empty_index]->trigger_explosion();
      active_explosion_pointers.insert(ptr_memory_pool[empty_index].get());
    }
    UNLOCK_MUTEX(ptr_mem_mutex);
  }

  void explosion::trigger_explosion(){
    if(halt_explosions){
      return;
    }
    m_debug("trigger_explosion()");
    done = false;
    phase = 0;
    start_tick = tick::get();
    if(phase >= self.bmp.size()){
      m_error("trigger_explosion tried to access invalid self.bmp at phase: " << phase);
    }else if(self.bmp[phase].texture == nullptr){
      m_error("self.bmp[phase].texture is nullptr. phase: " << phase);
    }else{
      SDL_RenderCopyEx(
          ren,  //renderer
          self.bmp[phase].texture,
          nullptr,// src rect
          &self.rect,
          rand_between(90,270),//angle, // angle
          nullptr,  // center
          SDL_FLIP_NONE // flip
          );
    }
  }
  void program_exit(){
    halt_explosions = true;
    //static std::set<explosion*> active_explosion_pointers;
    //static std::array<std::unique_ptr<explosion>,MAX_EXPLOSIONS_LIST_SIZE> ptr_memory_pool;
    LOCK_MUTEX(ptr_mem_mutex);
    for(size_t i=0; i < MAX_EXPLOSIONS_LIST_SIZE;i++){
      if(ptr_memory_pool[i] != nullptr){
        ptr_memory_pool[i]->self = {};
        ptr_memory_pool[i]->states.clear();
      }
      ptr_memory_pool[i] = nullptr;
    }
    UNLOCK_MUTEX(ptr_mem_mutex);
    LOCK_MUTEX(active_explosion_mutex);
    active_explosion_pointers.clear();
    UNLOCK_MUTEX(active_explosion_mutex);
  }
};


#undef m_debug
