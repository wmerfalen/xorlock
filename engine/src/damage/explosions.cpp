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
#include "../wall.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[DAMAGE][explosion][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[DAMAGE][explosion][ERROR]: " << A << "\n";

namespace damage::explosions {
  static bool halt_explosions = false;
  static int EXPLOSIONS_AUDIO_CHANNEL = 4;
  static int AMBIENCE_AUDIO_CHANNEL = 5;
  static constexpr std::size_t explosion_WIDTH = 375;
  static constexpr std::size_t explosion_HEIGHT = 260;
  static int mode = 0;
  static std::size_t EXPLOSION_DIR_START = 0;
  static std::size_t EXPLOSION_DIR_STOP = 3;
  static constexpr std::size_t MAX_EXPLOSIONS_LIST_SIZE = 40;
  static std::array<std::unique_ptr<explosion>,MAX_EXPLOSIONS_LIST_SIZE> ptr_memory_pool;

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

  static SDL_mutex* ptr_mem_mutex = SDL_CreateMutex();

  void move_map(int dir, int amount){
    if(halt_explosions){
      return;
    }
    LOCK_MUTEX(ptr_mem_mutex);
    for(auto& exp : ptr_memory_pool){
      if(!exp || exp->done){
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
    UNLOCK_MUTEX(ptr_mem_mutex);
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
    // TODO: load all explosion bmp's into memory
    for(size_t i=0; i < MAX_EXPLOSIONS_LIST_SIZE;i++){
      ptr_memory_pool[i] = nullptr;
    }
    for(size_t i=0; i < EXPLOSION_MAX;i++){
      std::string path = airstrike_dir.data();
      path += explosion_wav_files[i].first;
      path += ".wav";
      explosion_wav_files[i].second = Mix_LoadWAV(path.c_str());
      if(!explosion_wav_files[i].second){
        m_error("WARNING: couldn't load explosion wav file: " << explosion_wav_files[i].first);
      }
    }
    for(size_t i=0; i < AMBIENCE_MAX;i++){
      std::string path = airstrike_dir.data();
      path += ambience_wav_files[i].first;
      path += ".wav";
      ambience_wav_files[i].second = Mix_LoadWAV(path.c_str());
      if(!ambience_wav_files[i].second){
        m_error("WARNING: couldn't load ambience explosion wav file: " << ambience_wav_files[i].first);
      }
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

    LOCK_MUTEX(ptr_mem_mutex);
    bool must_cleanup = false;
    for(auto& ptr : ptr_memory_pool){
      if(!ptr || ptr->done){
        must_cleanup = true;
        continue;
      }
      ptr->tick();
    }
    UNLOCK_MUTEX(ptr_mem_mutex);
  }
  SDL_Texture* explosion::initial_texture() {
    m_debug("returning initial_texture: " << self.bmp.size());
    return self.bmp[0].texture;
  }
  explosion::explosion(uint8_t directory_id,SDL_Point* p,int in_radius,int in_damage,SDL_Rect source_rect) : 
    explosion(directory_id,p,in_radius,in_damage) {
      use_source_rect = true;
      m_source_rect = source_rect;

    }
  // TODO: add a function or parameter to constructor / initialize_with which:
  // 1) allows the caller to determine the size of the explosion (maybe determine this via damage?)
  // 2) allows caller to determine how soon the explosion goes away
  explosion::explosion(uint8_t directory_id,SDL_Point* p,int in_radius,int in_damage) : 
    type(directory_id), 
    angle(0), 
    explosive_damage(in_damage), 
    radius(in_radius), 
    x(p->x),y(p->y),
    done(false),
  use_source_rect(false){
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
  void explosion::initialize_with(uint8_t directory_id,SDL_Point* p,int in_radius,int damage,SDL_Rect source_rect) {
    initialize_with(directory_id,p,in_radius,damage);
    use_source_rect = true;
    m_source_rect = source_rect;
  }
  void explosion::initialize_with(uint8_t directory_id,SDL_Point* p,int in_radius,int damage) {
    if(halt_explosions){
      return;
    }
    angle = 0;
    explosive_damage = damage;
    radius = in_radius;
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
    if(start_tick + 200 < tick::get()){
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

	void detonate_from(SDL_Point* p,const uint16_t& radius, const uint16_t& damage,const uint8_t& type,SDL_Rect source_rect){
    if(halt_explosions){
      m_debug("halt_explosions preventing detonate_from");
      return;
    }
    m_debug("DETONATE_FROM: " << p->x << "," << p->y << " [" << damage << "](" << type << ")");

    LOCK_MUTEX(ptr_mem_mutex);
    for(size_t i=0; i < MAX_EXPLOSIONS_LIST_SIZE;i++){
      if(ptr_memory_pool[i] && ptr_memory_pool[i]->done){
        m_debug("found ->done with type: " << i);
        ptr_memory_pool[i]->initialize_with(type,p,radius,damage,source_rect);
        ptr_memory_pool[i]->trigger_explosion();
        UNLOCK_MUTEX(ptr_mem_mutex);
        return;
      }
      if(ptr_memory_pool[i] == nullptr){
        ptr_memory_pool[i] = std::make_unique<explosion>(type,p,radius,damage,source_rect);
        ptr_memory_pool[i]->trigger_explosion();
        UNLOCK_MUTEX(ptr_mem_mutex);
        return;
      }
    }
    m_debug("WARNING: stealing ptr_memory_pool");
    if(!ptr_memory_pool[0]){
      ptr_memory_pool[0] = std::make_unique<explosion>(type,p,radius,damage,source_rect);
    }
    ptr_memory_pool[0]->initialize_with(type,p,radius,damage,source_rect);
    ptr_memory_pool[0]->trigger_explosion();

    UNLOCK_MUTEX(ptr_mem_mutex);
  }
	void detonate_at(const Actor& a,const uint16_t& radius, const uint16_t& damage,const uint8_t& type){
    SDL_Point p{a.rect.x,a.rect.y};
    detonate_at(&p,radius,damage,type);
  }
  void detonate_at(SDL_Point* p,const uint16_t& radius, const uint16_t& damage,const uint8_t& type){
    if(halt_explosions){
      m_debug("halt_explosions preventing detonate_at");
      return;
    }
    m_debug("DETONATE_AT: " << p->x << "," << p->y << " [" << damage << "](" << type << ")");

    LOCK_MUTEX(ptr_mem_mutex);
    for(size_t i=0; i < MAX_EXPLOSIONS_LIST_SIZE;i++){
      if(ptr_memory_pool[i] && ptr_memory_pool[i]->done){
        m_debug("found ->done with type: " << i);
        ptr_memory_pool[i]->initialize_with(type,p,radius,damage);
        ptr_memory_pool[i]->trigger_explosion();
        UNLOCK_MUTEX(ptr_mem_mutex);
        return;
      }
      if(ptr_memory_pool[i] == nullptr){
        ptr_memory_pool[i] = std::make_unique<explosion>(type,p,radius,damage);
        ptr_memory_pool[i]->trigger_explosion();
        UNLOCK_MUTEX(ptr_mem_mutex);
        return;
      }
    }
    m_debug("WARNING: stealing ptr_memory_pool");
    if(!ptr_memory_pool[0]){
      ptr_memory_pool[0] = std::make_unique<explosion>(type,p,radius,damage);
    }
    ptr_memory_pool[0]->initialize_with(type,p,radius,damage);
    ptr_memory_pool[0]->trigger_explosion();

    UNLOCK_MUTEX(ptr_mem_mutex);
  }
  void play_random_explosion_wav(){
    Mix_PlayChannel(EXPLOSIONS_AUDIO_CHANNEL,explosion_wav_files[rand_between(0,EXPLOSION_MAX - 1)].second,0);
    Mix_PlayChannel(AMBIENCE_AUDIO_CHANNEL,ambience_wav_files[rand_between(0,AMBIENCE_MAX- 1)].second,0);
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
      return;
    }
    if(self.bmp[phase].texture == nullptr){
      m_error("self.bmp[phase].texture is nullptr. phase: " << phase);
      return;
    }
    SDL_RenderCopyEx(
        ren,  //renderer
        self.bmp[phase].texture,
        nullptr,// src rect
        &self.rect,
        rand_between(90,270),//angle, // angle
        nullptr,  // center
        SDL_FLIP_NONE // flip
        );
    play_random_explosion_wav();
#ifdef DRAW_EXPLOSION_RECT
    draw::rect(&self.rect);
#endif
    SDL_Rect res;
    for(const auto& n : world->npcs){
      // TODO: calculate explosion velocity
      // TODO: dynamically deal damage to targets depending on EV
      // TODO: throw npc corpses depending on EV
      if(SDL_IntersectRect(&n->rect,&self.rect,&res) == SDL_TRUE) {
#ifdef EXPLOSION_DAMAGE
        npc::take_explosive_damage(n,EXPLOSION_DAMAGE, //int damage,
                                                       &self.rect,//SDL_Rect* source_explosion,
                                                       rand_between(150,350), //int blast_radius,
                                                       0,
                                                       use_source_rect ? &m_source_rect : nullptr);                       //int on_death){
#else
        npc::take_explosive_damage(n,explosive_damage,
            &self.rect,
            rand_between(150,350),
            0,
            use_source_rect ? &m_source_rect : nullptr);                       //int on_death){
#endif
        // TODO: create "splattered" bits and pieces of the NPC's corpse
      }
    }
      if(SDL_IntersectRect(&plr::get()->self.rect,&self.rect,&res)){
#ifdef EXPLOSION_DAMAGE
        plr::take_explosive_damage(EXPLOSION_DAMAGE);
#else
        plr::take_explosive_damage(explosive_damage);
#endif
      }
  }
  void program_exit(){
    halt_explosions = true;
    LOCK_MUTEX(ptr_mem_mutex);
    for(size_t i=0; i < MAX_EXPLOSIONS_LIST_SIZE;i++){
      if(ptr_memory_pool[i] != nullptr){
        ptr_memory_pool[i]->self = {};
        ptr_memory_pool[i]->states.clear();
      }
      ptr_memory_pool[i] = nullptr;
    }
    UNLOCK_MUTEX(ptr_mem_mutex);
  }
};


#undef m_debug
