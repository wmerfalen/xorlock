#include "actor.hpp"
#include "asset.hpp"
#include <map>
#include <set>
#include <optional>
#include <functional>
#include "rng.hpp"

#ifdef FAIL
#undef FAIL
#endif

#ifdef DEBUG
#undef DEBUG
#endif

// FIXME
#define SHOW_FAILS
#ifdef SHOW_FAILS
#define FAIL(A) std::cerr << "FAIL(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
#else
#define FAIL(A)
#endif

#define DEBUG(A) std::cout << "DEBUG(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
extern SDL_Renderer* ren;
std::vector<SDL_Surface*> surface_list;
std::vector<SDL_Texture*> texture_list;
struct bmp_asset_t {
  std::string path;
  SDL_Surface* surface;
  SDL_Texture* texture;
  static std::optional<bmp_asset_t> create(std::string p){
    bmp_asset_t b;
    b.path = p;
    b.surface = SDL_LoadBMP(b.path.c_str());
    if(!b.surface){
      return std::nullopt;
    }
    b.texture = SDL_CreateTextureFromSurface(ren,b.surface);
    if(!b.texture){
      SDL_FreeSurface(b.surface);
      return std::nullopt;
    }
    surface_list.emplace_back(b.surface);
    texture_list.emplace_back(b.texture);
    return b;
  }
};
std::map<std::string,bmp_asset_t> bmp_library;
std::optional<bmp_asset_t> first_or_create(std::string p){
  if(bmp_library.find(p) != bmp_library.end()){
    DEBUG("CACHED: " << p);
    return bmp_library[p];
  }
  DEBUG("NOT CACHED YET: " << p);
  auto opt = bmp_asset_t::create(p);
  if(!opt.has_value()){
    DEBUG("UNABLE TO FIND: " << p);
    return std::nullopt;
  }
  bmp_library[p] = std::move(opt.value());
  return bmp_library[p];
}
Actor::Actor() {
  ready = 0;
  z = 0;
}
Actor::Actor(int32_t _x,int32_t _y,const char* _bmp_path) : rect({_x,_y,68,38}) {
  this->x = (_x);
  this->y = (_y);
  z = 0;
  ready = true;
  if(std::string(_bmp_path).find_first_of("%d") != std::string::npos) {
    load_bmp_assets(_bmp_path,360);
  } else {
    auto opt = first_or_create(_bmp_path);
    if(!opt.has_value()){
      FAIL("FAILED to load: '" << _bmp_path << "'");
      return;
    }
    bmp.emplace_back(opt.value().surface,opt.value().texture);
  }
}

std::pair<std::size_t,std::size_t> Actor::load_bmp_assets(const char* _bmp_path, std::size_t _bmp_count, std::size_t increment) {
  std::array<char,512> buf;
  std::size_t ok = 0, bad =0;
  for(std::size_t i=0; i < _bmp_count; i += increment) {
    std::fill(buf.begin(),buf.end(),0);
    int r = snprintf(&buf[0],sizeof(buf)-1,_bmp_path,i);
    if(r < 0) {
      FAIL("Couldn't build string using snprintf for _bmp_path: '" << _bmp_path << "' on the " << i << "th iteration");
      ++bad;
      continue;
    }
    std::string buf_path{&buf[0]};
    auto opt = first_or_create(buf_path);
    if(!opt.has_value()){
      FAIL("FAILED to load: '" << buf_path<< "'");
      continue;
    }
    bmp.emplace_back(opt.value().surface,opt.value().texture);
    ++ok;
    DEBUG("Loaded '" << buf_path << "'");
  }
  return {ok,bad};
}
bool Actor::load_bmp_asset(const char* _bmp_path) {
  auto opt = first_or_create(_bmp_path);
  if(!opt.has_value()){
    FAIL("FAILED to load: '" << _bmp_path<< "'");
    return false;
  }
  bmp.emplace_back(opt.value().surface,opt.value().texture);
  DEBUG("Loaded '" << _bmp_path << "'");
  return true;
}
std::pair<std::size_t,std::size_t> Actor::load_bmp_assets(const char* _bmp_path, std::size_t _bmp_count) {
  std::size_t ok = 0, bad =0;
  if(_bmp_count) {
    std::array<char,512> buf;
    for(std::size_t i=0; i < _bmp_count; ++i) {
      std::fill(buf.begin(),buf.end(),0);
      int r = snprintf(&buf[0],sizeof(buf)-1,_bmp_path,i);
      if(r < 0) {
        FAIL("Couldn't build string using snprintf for _bmp_path: '" << _bmp_path << "' on the " << i << "th iteration");
        ++bad;
        continue;
      }
      std::string buf_path{&buf[0]};
      auto opt = first_or_create(buf_path);
      if(!opt.has_value()){
        FAIL("FAILED to load: '" << buf_path<< "'");
        ++bad;
        continue;
      }
      bmp.emplace_back(opt.value().surface,opt.value().texture);
      ++ok;
      DEBUG("Loaded '" << buf_path << "'");
    }
    return {ok,bad};
  }
  std::string buf_path{_bmp_path};
  auto opt = first_or_create(buf_path);
  if(!opt.has_value()){
    FAIL("FAILED to load: '" << buf_path<< "'");
    ++bad;
  }else{
    bmp.emplace_back(opt.value().surface,opt.value().texture);
    ++ok;
  }
  DEBUG("Loaded '" << _bmp_path << "'");
  return {ok,bad};
}
//Actor::~Actor() {
//  //free_existing();
//}
void Actor::free_existing(){
  bmp.clear();
}
void Actor::calc() {
  this->cx = this->x + this->rect.w / 2;
  this->cy = this->y + this->rect.h / 2;
}

/** Copy constructor */
Actor::Actor(const Actor& other) {
  std::cout << "Actor::copy constructor\n";
  this->x = other.x;
  this->y = other.y;
  this->rect = other.rect;
  this->calc();// must be called after rect is assigned ALWAYS
  this->bmp = other.bmp;
  this->ready = other.ready;
  z = other.z;
}
std::string Actor::report() const {
  std::string s;
  s = "x: ";
  s += std::to_string(x);
  s += "y: ";
  s += std::to_string(y);
  s += "r.x: ";
  s += std::to_string(rect.x);
  s += "r.y: ";
  s += std::to_string(rect.y);
  s += "r.w: ";
  s += std::to_string(rect.w);
  s += "r.h: ";
  s += std::to_string(rect.h);
  return s;
}
void actor_program_exit(){
  /**
   * by running the code for about 2 seconds, and exiting, 
   * the body of this function accounts for atleast 87 allocations as of
   * Sat Dec  2 05:48:49 PM MST 2023.
   *
   * The base leaks that I can't seem to plug is 27.
   */
  bmp_library.clear();
  std::set<SDL_Surface*> freed_surfaces;
  for(const auto& surface : surface_list){
    if(freed_surfaces.find(surface) != freed_surfaces.end()){
      continue;
    }
    SDL_FreeSurface(surface);
    freed_surfaces.insert(surface);
  }
  freed_surfaces.clear();
  surface_list.clear();
  std::set<SDL_Texture*> freed_textures;
  for(const auto& texture : texture_list){
    if(freed_textures.find(texture) != freed_textures.end()){
      continue;
    }
    SDL_DestroyTexture(texture);
    freed_textures.insert(texture);
  }
  freed_textures.clear();
  texture_list.clear();
}
Asset* Actor::random_bmp(){
  for(auto& b : bmp){
    if(rng::chance(50)){
      return &b;
    }
  }
  return random_bmp();
}
