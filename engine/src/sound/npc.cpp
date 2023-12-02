#include "npc.hpp"
#include <pthread.h>
#include <mutex>
#include "../tick.hpp"
#include <SDL2/SDL_mixer.h>
#include "./directory.hpp"
#include <cassert>
#include "../rng.hpp"

#undef m_debug
#undef m_error

#define m_debug(A) std::cout << "[SOUND][NPC][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SOUND][NPC][ERROR]: " << A << "\n";
namespace sound::npc {
  using wav_list_t = std::vector<std::pair<std::string,Mix_Chunk*>>;
  static Mix_Chunk* mp5_reload = nullptr;
  static wav_list_t pain_list;
  static wav_list_t death_list;
  static wav_list_t corpse_list;
  static std::vector<Mix_Chunk*> pain_chunks;
  static std::vector<Mix_Chunk*> death_chunks;
  static std::vector<Mix_Chunk*> corpse_chunks;
  static std::size_t chunk_size;
  static std::size_t death_size;
  static std::size_t corpse_size;
  static constexpr int AUDIO_CHANNEL = 1;
  void free_lists(){
    for(const auto& p : pain_list){
      Mix_FreeChunk(p.second);
    }
    for(const auto& p : death_list){
      Mix_FreeChunk(p.second);
    }
    for(const auto& p : corpse_list){
      Mix_FreeChunk(p.second);
    }
    pain_list.clear();
    death_list.clear();
    corpse_list.clear();
    pain_chunks.clear();
    death_chunks.clear();
    corpse_chunks.clear();
    chunk_size = death_size = corpse_size = 0;
  }

  void load_sounds(){
    m_debug("load_sounds");
    free_lists();

    dir::load_folder(constants::npc_pain_dir,&pain_list);
    dir::load_folder(constants::npc_death_dir,&death_list);
    dir::load_folder(constants::npc_corpse_dir,&corpse_list);
    for(const auto& p : pain_list){
      std::cout << "p: (pain_list) '" << p.first << "':=>'" << p.second << "'\n";
      pain_chunks.emplace_back(p.second);
    }
    for(const auto& p : death_list){
      std::cout << "p: (death_list) '" << p.first << "':=>'" << p.second << "'\n";
      death_chunks.emplace_back(p.second);
    }
    for(const auto& p : corpse_list){
      std::cout << "p: (corpse_list) '" << p.first << "':=>'" << p.second << "'\n";
      corpse_chunks.emplace_back(p.second);
    }
    chunk_size = pain_chunks.size();
    death_size = death_chunks.size();
    corpse_size = corpse_chunks.size();
  }
  void init(){
    m_debug("[init()]: initializing sound assets");
    load_sounds();
    m_debug(pain_list.size() << " (pain_list) wav reload files loaded");
    m_debug(death_list.size() << " (death_list) wav reload files loaded");
    m_debug(corpse_list.size() << " (corpse_list) wav reload files loaded");
  }
  void stop(){
    Mix_FadeOutChannel(0,500);
  }
  void play_npc_pain(const int& npc_type){
    std::size_t i = rand_xoroshiro() % chunk_size;
    Mix_PlayChannel(AUDIO_CHANNEL,pain_chunks[i],0);
  }
  void play_death_sound(const int& npc_type){
    std::size_t i = rand_xoroshiro() % death_size;
    Mix_PlayChannel(AUDIO_CHANNEL,death_chunks[i],0);
  }
  void play_corpse_sound(const int& npc_type,const int& hp){
    std::size_t i = rand_xoroshiro() % corpse_size;
    Mix_PlayChannel(AUDIO_CHANNEL,corpse_chunks[i],0);
  }
  void program_exit(){
    free_lists();
  }
};
