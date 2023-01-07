#ifndef __ACTOR_HEADER__
#define __ACTOR_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <array>

#include "asset.hpp"

#ifdef FAIL
#undef FAIL
#endif

#ifdef DEBUG
#undef DEBUG
#endif

#define FAIL(A) std::cerr << "FAIL(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
#define DEBUG(A) std::cout << "DEBUG(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
extern SDL_Renderer* ren;

struct Actor {
  std::pair<std::size_t,std::size_t> load_bmp_assets(const char* _bmp_path, std::size_t _bmp_count, std::size_t increment) {
    std::array<char,512> buf;
    std::size_t ok = 0, bad =0;
    for(std::size_t i=0; i < _bmp_count; i += increment){
      std::fill(buf.begin(),buf.end(),0);
      int r = snprintf(&buf[0],sizeof(buf)-1,_bmp_path,i);
      if(r < 0){
        FAIL("Couldn't build string using snprintf for _bmp_path: '" << _bmp_path << "' on the " << i << "th iteration");
        ++bad;
        continue;
      }
      auto p = SDL_LoadBMP(&buf[0]);
      if(p == nullptr){
        FAIL("Couldn't load asset: '" << &buf[0] << "'");
        ++bad;
        continue;
      }
      auto tex = SDL_CreateTextureFromSurface(ren, p);
      if(tex == nullptr){
        SDL_FreeSurface(p);
        ++bad;
        continue;
      }
      bmp.emplace_back(p,tex);
      ++ok;
      DEBUG("Loaded '" << &buf[0] << "'");
    }
    return {ok,bad};
  }
  bool load_bmp_asset(const char* _bmp_path){
    auto p = SDL_LoadBMP(_bmp_path);
    if(p == nullptr){
      FAIL("Couldn't load asset: '" << _bmp_path << "'");
      return false;
    }
    auto tex = SDL_CreateTextureFromSurface(ren, p);
    if(tex == nullptr){
      SDL_FreeSurface(p);
      return false;
    }
    bmp.emplace_back(p,tex);
    DEBUG("Loaded '" << _bmp_path << "'");
    return true;
  }
  std::pair<std::size_t,std::size_t> load_bmp_assets(const char* _bmp_path, std::size_t _bmp_count) {
    if(_bmp_count){
      std::array<char,512> buf;
      std::size_t ok = 0, bad =0;
      for(std::size_t i=0; i < _bmp_count;++i){
        std::fill(buf.begin(),buf.end(),0);
        int r = snprintf(&buf[0],sizeof(buf)-1,_bmp_path,i);
        if(r < 0){
          FAIL("Couldn't build string using snprintf for _bmp_path: '" << _bmp_path << "' on the " << i << "th iteration");
          ++bad;
          continue;
        }
        auto p = SDL_LoadBMP(&buf[0]);
        if(p == nullptr){
          FAIL("Couldn't load asset: '" << &buf[0] << "'");
          ++bad;
          continue;
        }
        auto tex = SDL_CreateTextureFromSurface(ren, p);
        if(tex == nullptr){
          SDL_FreeSurface(p);
          ++bad;
          continue;
        }
        bmp.emplace_back(p,tex);
        ++ok;
        DEBUG("Loaded '" << &buf[0] << "'");
      }
      return {ok,bad};
    }
    auto p = SDL_LoadBMP(_bmp_path);
    if(p == nullptr){
      FAIL("Couldn't load asset: '" << _bmp_path << "'");
      return {0,1};
    }
    auto tex = SDL_CreateTextureFromSurface(ren, p);
    if(tex == nullptr){
      return {0,2};
    }
    bmp.emplace_back(p,tex);
    DEBUG("Loaded '" << _bmp_path << "'");
    return {1,0};
  }
  Actor(int32_t _x,int32_t _y,const char* _bmp_path) : 
    x(_x),
    y(_y),
    rect({_x,_y,68,38}),
    ready(true){
      if(std::string(_bmp_path).find_first_of("%d") != std::string::npos) {
        load_bmp_assets(_bmp_path,360);
      }else{
        auto p = SDL_LoadBMP(_bmp_path);
        if(p == nullptr){
          FAIL("Couldn't load asset: '" << _bmp_path << "'");
          return;
        }
        auto tex = SDL_CreateTextureFromSurface(ren, p);
        bmp.emplace_back(p,tex);
        DEBUG("Loaded '" << _bmp_path << "'");
      }
    }
  ~Actor() {
    if(ready){
      for(const auto& b : bmp){
        if(b.surface){
          SDL_FreeSurface(b.surface);
        }
        if(b.texture){
          SDL_DestroyTexture(b.texture);
        }
      }
    }
  }
  int x;
  int y;
  int cx;
  int cy;
  void calc(){
    cx = x + rect.w / 2;
    cy = y + rect.h / 2;
  }

  std::vector<Asset> bmp;
  SDL_Rect rect;
  bool ready;

  Actor() : ready(false) {}
  /** Copy constructor */
  Actor(const Actor& other){
    std::cout << "Actor::copy constructor\n";
    x = other.x;
    y = other.y;
    rect = other.rect;
    calc();// must be called after rect is assigned ALWAYS
    bmp = other.bmp;
    ready = other.ready;
  }
  std::string report() const {
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
};


#endif
