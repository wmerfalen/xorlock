#ifndef __BACKGROUND_HEADER__
#define __BACKGROUND_HEADER__
#include <SDL2/SDL.h>
#include "extern.hpp"
#include <memory>
#include <array>

using Color = std::array<uint8_t,3>;
static constexpr Color DEFAULT_BG = {0,0,0};

struct Background {
  Color color;
  uint64_t width;
  uint64_t height;
  Background() = delete;
  Background(const Background&) = delete;
  Background(const Color& a) : color(a) {
    width = win_width();
    height = win_height();
  }
  void draw(){
    SDL_SetRenderDrawColor(
        ren,
        color[0],
        color[1],
        color[2],
        0);
    ren_clear();
  }

  ~Background() = default;

};

namespace bg {
  static std::unique_ptr<Background> background;

  void init(){
    background = std::make_unique<Background>(DEFAULT_BG);
    background->draw();
  }

  void draw(){
  }
};

#endif
