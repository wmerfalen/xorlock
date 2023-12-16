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
#include <optional>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[DRAW_STATE][BACKPACK][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[DRAW_STATE][BACKPACK][ERROR]: " << A << "\n";

extern int WIN_WIDTH;
extern int WIN_HEIGHT;
namespace draw_state::backpack {
  static bool m_draw_backpack;
  uint64_t debounce_return = 0;
  uint32_t current_selection = 0;
  SDL_Rect backpack_display_rect{0,0,500,500};
  SDL_Point p{0,0};
  SDL_Point p_status{(WIN_WIDTH / 32) + ((WIN_WIDTH / 32) * 2),(WIN_HEIGHT / 32) };
  SDL_Rect p_status_rect{WIN_WIDTH / 64,WIN_HEIGHT /64,(WIN_WIDTH / 64) * 30,50};
  int p_status_height = 20;
  uint8_t p_status_alert_color[] = {0x6a,0x6b,0x62};
  enum menu_t : uint8_t {
    MENU_PRIMARY = 0,
    MENU_SECONDARY,
    MENU_FRAG,
    MENU_MAX_SIZE,
  };
  menu_t current_menu = menu_t::MENU_PRIMARY;
  std::string display_alert;
  uint64_t display_alert_until = 0;
  std::string display_success;
  uint64_t display_success_until = 0;
  uint8_t backpack_bg_color[] = {0x6a,0x6b,0x62};
  uint64_t debounce_down = 0;
  uint64_t debounce_up = 0;
  uint64_t debounce_escape = 0;
  uint64_t debounce_primary = 0;
  uint64_t debounce_secondary = 0;
  uint64_t debounce_frag = 0;
  uint64_t debounce_delete = 0;
  uint64_t debounce_question = 0;
  bool display_question = false;
  std::string question;
  std::optional<bool> answer;
  std::optional<std::function<void(void)>> handle_answer;
	void init() {
    m_draw_backpack = 0;
    answer = std::nullopt;
    handle_answer = std::nullopt;
	}
  bool is_primary(const loot::ExportWeapon* ptr){
    switch(ptr->type){
      default:
        return false;
      case wpn::weapon_type_t::WPN_T_AR:
      case wpn::weapon_type_t::WPN_T_SMG:
      case wpn::weapon_type_t::WPN_T_SHOTGUN:
      case wpn::weapon_type_t::WPN_T_SNIPER:
      case wpn::weapon_type_t::WPN_T_LMG:
      case wpn::weapon_type_t::WPN_T_DMR:
        return true;
    }
  }
  bool is_secondary(const loot::ExportWeapon* ptr){
    switch(ptr->type){
      default:
        return false;
      case wpn::weapon_type_t::WPN_T_PISTOL:
      case wpn::weapon_type_t::WPN_T_MACHINE_PISTOL:
        return true;
    }
  }
  loot::ExportWeapon* get_weapon_at_current_selection(){
    std::vector<size_t> primary_weapon_index;
    std::vector<size_t> secondary_weapon_index;
    size_t count = plr::get()->backpack->weapons_ptr.size();
    size_t i=0;
    std::vector<size_t>* menu_items = nullptr;
      for(i=0;i < count;i++){
        auto ptr = plr::get()->backpack->weapons_ptr[i];
        if(current_menu == MENU_PRIMARY && is_primary(ptr)){
          primary_weapon_index.emplace_back(i);
        }
        if(current_menu == MENU_SECONDARY && is_secondary(ptr)){
          secondary_weapon_index.emplace_back(i);
        }
      }
      if(current_menu == MENU_PRIMARY){
        count = primary_weapon_index.size();
        menu_items = &primary_weapon_index;
      }else{
        count = secondary_weapon_index.size();
        menu_items = &secondary_weapon_index;
      }
      if(current_selection >= count){
        m_debug("current_selection greater than count");
        return nullptr;
      }
      if(!menu_items){
        m_debug("null menu_items");
        return nullptr;
      }
      if(menu_items->size()){
        if(current_selection >= menu_items->size()){
          return nullptr;
        }
        return plr::get()->backpack->weapons_ptr[current_selection];
      }
      m_debug("last chance");
      return nullptr;
  }
  loot::ExportGrenade* get_frag_at_current_selection(){
    size_t i=0;
    for(; i < plr::get()->backpack->grenades_ptr.size();i++){
      if(current_selection == i){
        return plr::get()->backpack->grenades_ptr[i];
      }
    }
    return nullptr;
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

  }
  void toggle_menu(){
    m_draw_backpack = !m_draw_backpack;
  }
  void start_menu(){
    debounce_escape = tick::get() + 400;
  }
  void reset_displays(){
    display_alert_until = 0;
    display_success_until = 0;
  }
  void alert(std::string_view msg){
    display_alert_until = tick::get() + 1500;
    display_alert = msg.data();
  }
  void success(std::string_view msg){
    display_success_until = tick::get() + 1500;
    display_success = msg.data();
  }
  template <typename TLambda>
  void confirm(std::string msg,TLambda func){
    display_question = true;
    question = msg.data();
    answer = std::nullopt;
    handle_answer = func;
  }
  void handle_key_press(){
    auto keys = SDL_GetKeyboardState(nullptr);
    if(display_question && (keys[SDL_SCANCODE_Y] || keys[SDL_SCANCODE_N] || keys[SDL_SCANCODE_RETURN]) && debounce_question < tick::get()){
      sound::menu::play_menu_change();
      reset_displays();
      debounce_question = tick::get() + 1000;
      if(keys[SDL_SCANCODE_Y] || keys[SDL_SCANCODE_RETURN]){
        handle_answer.value()();
      }
      display_question = false;
      debounce_return = tick::get() + 1000;
      return;
    }
    if(keys[SDL_SCANCODE_P] && debounce_primary < tick::get()){
      sound::menu::play_menu_change();
      debounce_primary = tick::get() + 80;
      current_menu = menu_t::MENU_PRIMARY;
      reset_displays();
      current_selection = 0;
      return;
    }
    if(keys[SDL_SCANCODE_S] && debounce_secondary < tick::get()){
      sound::menu::play_menu_change();
      debounce_secondary = tick::get() + 80;
      current_menu = menu_t::MENU_SECONDARY;
      reset_displays();
      current_selection = 0;
      return;
    }
    if(keys[SDL_SCANCODE_F] && debounce_frag < tick::get()){
      sound::menu::play_menu_change();
      debounce_frag = tick::get() + 80;
      current_menu = menu_t::MENU_FRAG;
      reset_displays();
      current_selection = 0;
      return;
    }

    if(keys[SDL_SCANCODE_DOWN] && debounce_down < tick::get()){
      ++current_selection;
      // TODO: handle wrapping
      sound::menu::play_menu_change();
      debounce_down = tick::get() + 80;
      reset_displays();
      return;
    }
    if(keys[SDL_SCANCODE_UP] && debounce_up < tick::get()){
      if(current_selection - 1 < 0){
        current_selection = 0;
      }else{
        --current_selection;
      }
      // TODO: handle wrapping
      sound::menu::play_menu_change();
      debounce_up = tick::get() + 80;
      reset_displays();
      return;
    }
    if((keys[SDL_SCANCODE_TAB] || keys[SDL_SCANCODE_ESCAPE]) && debounce_escape < tick::get()){
      current_selection = 0;
      toggle_menu();
      debounce_escape = tick::get() + 800;
      reset_displays();
      return;
    }
    if(keys[SDL_SCANCODE_RETURN] && debounce_return < tick::get()){
      reset_displays();
      std::pair<bool,std::string> status;
      switch(current_menu){
        case MENU_PRIMARY:
          status = plr::get()->backpack->wield_primary(get_weapon_at_current_selection());
          break;
        case MENU_SECONDARY:
          status = plr::get()->backpack->wield_secondary(get_weapon_at_current_selection());
          break;
        case MENU_FRAG:
          status = plr::get()->backpack->wield_frag(get_frag_at_current_selection());
          break;
        default:
          m_debug("invalid menu item");
          break;
      }
      if(status.first){
        display_success_until = tick::get() + 2000;
        display_success = status.second;
      }else{
        display_alert_until = tick::get() + 2000;
        display_alert = status.second;
      }
      debounce_return = tick::get() + 800;
    }
    if((keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_DELETE]) && debounce_delete < tick::get()){
      auto ptr = get_weapon_at_current_selection();
      auto frag_ptr = get_frag_at_current_selection();
      switch(current_menu){
        case MENU_PRIMARY:
        case MENU_SECONDARY: 
          if(ptr == nullptr){
            alert("No weapon at current selection");
          }else{
            confirm("Are you sure you want to drop this item?",[&]() {
                auto ptr = get_weapon_at_current_selection();
                plr::get()->backpack->remove_item(ptr->id);
                plr::get()->backpack->refresh();
                success("dropped item");
                });
          }
          break;
        case MENU_FRAG: 
          if(frag_ptr == nullptr){
            alert("No frag at current selection");
          }else{
            confirm("Are you sure you want to drop this item?",[&]() {
                auto ptr = get_frag_at_current_selection();
                plr::get()->backpack->remove_item(ptr->id);
                plr::get()->backpack->refresh();
                success("dropped item");
                });
          }
          break;
        default:
          m_debug("invalid menu item");
          break;
      }

      debounce_delete = tick::get() + 800;
      reset_displays();
      return;
    }
  }
  template <typename T>
    std::string name(const T* ptr){
      return std::string(&ptr->name[0]);
    }
  size_t offset = 0;
  void draw_sub_menu(SDL_Renderer* ren){
    SDL_Point p{0,0};
    static constexpr int height = 20;

    std::string m;
    for(uint8_t i=0; i < (uint8_t)menu_t::MENU_MAX_SIZE;i++){
      switch((menu_t)i){
        case MENU_PRIMARY:
          m = "[p]rimary";
          break;
        case MENU_SECONDARY:
          m = "[s]secondary";
          break;
        case MENU_FRAG:
          m = "[f]rag";
          break;
        default:
          break;
      }
      if(current_menu == i){
        font::small_green_text(&p,//const SDL_Point* where,
            m,//const std::string& msg,
            height);  //int height);
      }else{
        font::small_white_text(&p,//const SDL_Point* where,
            m,//const std::string& msg,
            height);  //int height);
      }
      p.y += height;
    }
  }

  void draw_menu(SDL_Renderer* ren){
    save_draw_color();
    set_draw_color("red");
    draw_sub_menu(ren);
    SDL_Point p{0,80};
    SDL_Point details{250,0};
    size_t i=0;
    size_t count = plr::get()->backpack->weapons_ptr.size();
    size_t gren_count = plr::get()->backpack->grenades_ptr.size();
    std::vector<size_t> primary_weapon_index;
    std::vector<size_t> secondary_weapon_index;
    std::vector<size_t>* menu_items = nullptr;
    if(current_menu <= MENU_SECONDARY){
      for(i=offset;i < count;i++){
        auto ptr = plr::get()->backpack->weapons_ptr[i];
        if(current_menu == MENU_PRIMARY && is_primary(ptr)){
          primary_weapon_index.emplace_back(i);
        }
        if(current_menu == MENU_SECONDARY && is_secondary(ptr)){
          secondary_weapon_index.emplace_back(i);
        }
      }
      if(current_menu == MENU_PRIMARY){
        count = primary_weapon_index.size();
        menu_items = &primary_weapon_index;
      }else{
        count = secondary_weapon_index.size();
        menu_items = &secondary_weapon_index;
      }
      if(current_selection >= count){
        current_selection = 0;
      }
      if(menu_items->size()){
        for(i=0; i < menu_items->size();i++){
          auto ptr = plr::get()->backpack->weapons_ptr[menu_items->at(i)];
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
        auto ptr = plr::get()->backpack->weapons_ptr[current_selection % count];
        i = 0;
        for(const auto& line : wpn_info::weapon_stats(&ptr->stats)){
          font::small_green_text(&details,//const SDL_Point* where,
              line,//const std::string& msg,
              40);  //int height);
          details.y += 40;
        }
      }
    } else{
      if(gren_count){
        for(i=offset;i < gren_count;i++){
          auto ptr = plr::get()->backpack->grenades_ptr[i];
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
        auto ptr = plr::get()->backpack->grenades_ptr[current_selection % gren_count];
        i = 0;
        for(const auto& line : wpn_info::explosive_stats(&ptr->stats)){
          font::small_green_text(&details,//const SDL_Point* where,
              line,//const std::string& msg,
              40);  //int height);
          details.y += 40;
        }
      }
    }
    if(display_alert_until > tick::get()){
      SDL_SetRenderDrawColor(ren,p_status_alert_color[0],p_status_alert_color[1],p_status_alert_color[2],0);
      SDL_RenderFillRect(ren,&p_status_rect);
      font::small_red_text(&p_status,
          display_alert,
          p_status_height);
    }
    if(display_success_until > tick::get()){
      SDL_SetRenderDrawColor(ren,p_status_alert_color[0],p_status_alert_color[1],p_status_alert_color[2],0);
      SDL_RenderFillRect(ren,&p_status_rect);
      font::small_green_text(&p_status,
          display_success,
          p_status_height
          );
    }
    if(display_question){
      auto pr = p_status_rect;
      SDL_SetRenderDrawColor(ren,p_status_alert_color[0],p_status_alert_color[1],p_status_alert_color[2],0);
      pr.w *= 2;
      pr.h *= 3;
      SDL_RenderFillRect(ren,&pr);
      auto p = p_status;
      font::small_red_text(&p,
          question,
          p_status_height);
      p.y += p_status_height;
      font::small_red_text(&p,
          "[Y]es",
          p_status_height);
      p.y += p_status_height;
      font::small_green_text(&p,
          "[N]o",
          p_status_height);
      SDL_SetRenderDrawColor(ren,p_status_alert_color[0],p_status_alert_color[1],p_status_alert_color[2],0);
    }
    {
      auto p = p_status;
      p.y *= 2;
      font::small_green_text(&p,
          std::to_string(current_selection),
          p_status_height);
    }
    restore_draw_color();
    handle_key_press();
    SDL_PumpEvents();
  }
};
