#include <iostream>
#include "player.hpp"
#include "../player.hpp"
#include <SDL2/SDL.h>
#include "../font.hpp"

extern int32_t START_X;
extern int32_t START_Y;
namespace draw_state::player {
	static bool m_draw_guy;
  static bool m_show_hp;
  static SDL_Point m_hp_point;
  static int16_t* hp;
  static std::string hp_string;
	void init() {
		m_draw_guy = 1;
    show_hp(true);
    hp = &plr::get()->hp;
	}
  void tick(){
    if(m_show_hp){
      m_hp_point.x = plr::cx() - 250;
      m_hp_point.y = 0;
      hp_string = std::to_string(*hp);
      if(*hp < 20){
        font::small_red_text(&m_hp_point,hp_string,55);
      }else{
        font::small_green_text(&m_hp_point,hp_string,55);
      }
    }
  }
	bool draw_guy() {
		return m_draw_guy;
	}

	void hide_guy() {
		m_draw_guy = false;
	}
	void show_guy() {
		m_draw_guy = true;
	}

  void show_hp(bool in_show){
    m_show_hp = in_show;
  }

};
