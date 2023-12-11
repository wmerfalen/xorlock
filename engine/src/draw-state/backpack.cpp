#include <iostream>
#include "backpack.hpp"
#include "../draw.hpp"
#include "../font.hpp"
#include <SDL2/SDL.h>

namespace draw_state::backpack {
  static bool m_draw_backpack;
  SDL_Rect backpack_display_rect{0,0,500,500};
  SDL_Point p{0,0};
	void init() {
    m_draw_backpack = 0;
	}

	bool draw_backpack(){
    return m_draw_backpack;
  }
  void show_backpack(){
    m_draw_backpack = true;
  }
  void hide_backpack(){
    m_draw_backpack = false;
  }

  void tick(){
    if(m_draw_backpack == false){
      return;
    }
    draw::blatant_rect(&backpack_display_rect);
    font::small_red_text(&p,"mp5 [elite]",20);
  }

};
