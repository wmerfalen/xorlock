#include "menu.hpp"
#include "directory.hpp"
#include "../tick.hpp"
#include <SDL2/SDL_mixer.h>
#include <sys/types.h>
#include <dirent.h>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[SOUND][MENU][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SOUND][MENU][ERROR]: " << A << "\n";
namespace sound::menu {
  static constexpr int MENU_AUDIO_CHANNEL = 0;

  using wav_list_t = std::vector<std::pair<std::string,Mix_Chunk*>>;
  static wav_list_t menu_list;
  static Mix_Chunk* menu_next = nullptr;
  static Mix_Chunk* menu_select = nullptr;
  std::size_t reload_menus(){
    m_debug("reload_menus");
    for(auto& p : menu_list){
      Mix_FreeChunk(p.second);
      p.second = nullptr;
    }
    if(menu_next){
      Mix_FreeChunk(menu_next);
    }
    menu_next = nullptr;
    if(menu_select){
      Mix_FreeChunk(menu_select);
    }
    menu_select = nullptr;
    menu_list.clear();
    sound::dir::load_folder(constants::menu_dir,&menu_list);
    std::string next = constants::menu_next_wave;
    next += ".wav";
    std::string select = constants::menu_select_wave;
    select += ".wav";
    for(const auto& p : menu_list){
      if(p.first.compare(next.c_str()) == 0){
        m_debug("menu_next_wave found");
        menu_next = p.second;
        Mix_VolumeChunk(menu_next,25);
      }
      if(p.first.compare(select.c_str()) == 0){
        m_debug("menu_select_wave found");
        menu_select = p.second;
        Mix_VolumeChunk(menu_select,25);
      }
    }
    return menu_list.size();
  }
  void init(){
    m_debug("[init()]: initializing sound assets");
    m_debug(reload_menus() << " wav menu files loaded");
  }
  void play_menu_change(){
    m_debug("play_menu_change - this thread id: " << SDL_GetThreadID(nullptr));
    Mix_PlayChannel(MENU_AUDIO_CHANNEL,menu_next,0);
  }
  void play_menu_select_item(){
    Mix_PlayChannel(MENU_AUDIO_CHANNEL,menu_select,0);
  }
  void program_exit(){
    m_debug("program_exit");
    m_debug("program_exit - this thread id: " << SDL_GetThreadID(nullptr));
    if(menu_list.size()){
      //for(auto& p : menu_list){
      //  if(p.second == nullptr){
      //    continue;
      //  }
      //  Mix_FreeChunk(p.second);
      //  p.second = nullptr;
      //}
      menu_list.clear();
    }
    if(menu_next){
      Mix_FreeChunk(menu_next);
      menu_next = nullptr;
    }
    if(menu_select){
      Mix_FreeChunk(menu_select);
      menu_select = nullptr;
    }
  }
};
