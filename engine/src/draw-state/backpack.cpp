#include <iostream>
#include "backpack.hpp"
#include "../draw.hpp"
#include "../font.hpp"
#include "../tick.hpp"
#include "../sound/menu.hpp"
#include "../extern.hpp"
#include "../player.hpp"
#include "../loot.hpp"
#include <SDL2/SDL.h>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[DRAW_STATE][BACKPACK][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[DRAW_STATE][BACKPACK][ERROR]: " << A << "\n";
namespace draw_state::backpack {
  static bool m_draw_backpack;
  uint32_t current_selection = 0;
  SDL_Rect backpack_display_rect{0,0,500,500};
  SDL_Point p{0,0};
  uint8_t backpack_bg_color[] = {0x6a,0x6b,0x62};
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
    draw::fill_rect(&backpack_display_rect,backpack_bg_color);

    font::small_red_text(&p,"mp5 [elite]",20);
  }
  uint64_t debounce_down = 0;
  uint64_t debounce_up = 0;
  uint64_t debounce_escape = 0;
  void toggle_menu(){
    m_draw_backpack = !m_draw_backpack;
  }
  void start_menu(){
    debounce_escape = tick::get() + 400;
  }
  void handle_key_press(){
    auto keys = SDL_GetKeyboardState(nullptr);

    if(keys[SDL_SCANCODE_DOWN] && debounce_down < tick::get()){
      ++current_selection;
      // TODO: handle wrapping
      m_debug("down - " << tick::get());
      m_debug("down - this thread id: " << SDL_GetThreadID(nullptr));
      sound::menu::play_menu_change();
      debounce_down = tick::get() + 80;
      return;
    }
    if(keys[SDL_SCANCODE_UP] && debounce_up < tick::get()){
      --current_selection;
      // TODO: handle wrapping
      m_debug("up - this thread id: " << SDL_GetThreadID(nullptr));
      sound::menu::play_menu_change();
      m_debug("up - " << tick::get());
      debounce_up = tick::get() + 80;
      return;
    }
    if((keys[SDL_SCANCODE_TAB] || keys[SDL_SCANCODE_ESCAPE]) && debounce_escape < tick::get()){
      m_debug("escape - " << tick::get());
      toggle_menu();
      debounce_escape = tick::get() + 800;
      return;
    }
    if(keys[SDL_SCANCODE_RETURN]){
      // TODO: handle return
    }
  }
  template <typename T>
  std::string name(const T* ptr){
    return std::string(&ptr->name[0]);
  }
  size_t offset = 0;

  void draw_menu(SDL_Renderer* ren){
    save_draw_color();
    set_draw_color("red");
    SDL_Point p{0,0};
    SDL_Point details{250,0};
    size_t i=0;
    size_t count = plr::get()->backpack->weapons_ptr.size();
    for(i=offset;i < count;i++){
      auto ptr = plr::get()->backpack->weapons_ptr[i];
      std::string m;
      if(current_selection == i){
        m = "> ";
        m += name(ptr);
      }else{
        m = name(ptr);
      }
      font::small_white_text(&p,//const SDL_Point* where,
          m,//const std::string& msg,
          20);  //int height);
      p.y += 20;
    }
    auto ptr = plr::get()->backpack->weapons_ptr[current_selection];
    i = 0;
    for(const auto& line : wpn_info::weapon_stats(&ptr->stats)){
      font::small_green_text(&details,//const SDL_Point* where,
          line,//const std::string& msg,
          40);  //int height);
      details.y += 40;
    }
    restore_draw_color();
    handle_key_press();
    SDL_PumpEvents();
  }
};
