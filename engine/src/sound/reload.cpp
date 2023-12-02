#include "reload.hpp"
#include <pthread.h>
#include <mutex>
#include "../tick.hpp"
#include <SDL2/SDL_mixer.h>
#include "./directory.hpp"
#include <cassert>

#undef m_debug
#undef m_error

#define m_debug(A) std::cout << "[SOUND][RELOAD][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SOUND][RELOAD][ERROR]: " << A << "\n";
namespace sound::reload {
  using wav_list_t = std::vector<std::pair<std::string,Mix_Chunk*>>;
  static constexpr std::size_t RELOAD_AUDIO_CHANNEL = 0;
  static Mix_Chunk* mp5_reload = nullptr;
  enum reload_phase_t : uint16_t {
    EJECT_MAG =0,
    PULL_REPLACEMENT_MAG,
    LOAD_MAG,
    WEAPON_SLIDE,
    __RELOAD_PHASE_COUNT = WEAPON_SLIDE + 1,
  };
  std::array<const char*,reload_phase_t::__RELOAD_PHASE_COUNT> reload_phase_file_names = {
    "eject-mag-0",
    "replace-mag-0",
    "load-mag-0",
    "weapon-slide-0",
  };
  std::array<Mix_Chunk*,reload_phase_t::__RELOAD_PHASE_COUNT> reload_phase;
  static wav_list_t reload_list;
  void load_reload(){
    m_debug("load_reload");

    reload_list.clear();
    dir::load_folder(constants::reload_dir,&reload_list);
    mp5_reload = nullptr;
    for(std::size_t i=0; i < reload_phase_t::__RELOAD_PHASE_COUNT; i++){
      reload_phase[i] = nullptr;
    }
    for(const auto& p : reload_list){
      std::cout << "p: '" << p.first << "':=>'" << p.second << "'\n";
      std::string tmp;
      tmp = p.first;
      if(tmp.compare(constants::mp5_reload_wave) == 0){
        m_debug("mp5_reload_wave found");
        mp5_reload = p.second;
      }
      for(std::size_t i=0; i < reload_phase_t::__RELOAD_PHASE_COUNT; i++){
        std::string t = reload_phase_file_names[i];
        t += ".wav";
        if(tmp.compare(t.c_str()) == 0){
          reload_phase[i] = p.second;
        }
      }
    }
    for(std::size_t i=0; i < reload_phase_t::__RELOAD_PHASE_COUNT; i++){
      assert(reload_phase[i] != nullptr);
    }
  }
  void init(){
    m_debug("[init()]: initializing sound assets");
    load_reload();
    m_debug(reload_list.size() << " wav reload files loaded");
  }
  void stop_mp5_reload(){
    Mix_FadeOutChannel(RELOAD_AUDIO_CHANNEL,500);
  }
  void play_eject(){
    Mix_PlayChannel(RELOAD_AUDIO_CHANNEL,reload_phase[reload_phase_t::EJECT_MAG],0);
  }
  void play_pull_replacement_mag(){
    Mix_PlayChannel(RELOAD_AUDIO_CHANNEL,reload_phase[reload_phase_t::PULL_REPLACEMENT_MAG],0);
  }
  void play_load_mag(){
    Mix_PlayChannel(RELOAD_AUDIO_CHANNEL,reload_phase[reload_phase_t::LOAD_MAG],0);
  }
  void play_weapon_slide(){
    Mix_PlayChannel(RELOAD_AUDIO_CHANNEL,reload_phase[reload_phase_t::WEAPON_SLIDE],0);
  }
  void program_exit(){
    for(const auto& p : reload_list){
      Mix_FreeChunk(p.second);
    }
    reload_list.clear();
  }
};
