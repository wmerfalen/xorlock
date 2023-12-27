#include "gunshot.hpp"
#include "./directory.hpp"
#include "./reload.hpp"
#include <pthread.h>
#include <mutex>
#include "../tick.hpp"
#include <SDL2/SDL_mixer.h>
#include <sys/types.h>
#include <dirent.h>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[SOUND][GUNSHOT][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SOUND][GUNSHOT][ERROR]: " << A << "\n";
//std::array<Mix_Chunk*,wpn::weapon_t::__WPN_SIZE> weapon_waves;
namespace sound {
  static constexpr int GUNSHOT_AUDIO_CHANNEL = 0;
  static constexpr int RELOAD_AUDIO_CHANNEL = 0;
  static constexpr int CYCLE_CHANNEL = 4;
  static constexpr int SPAS12_CHANNEL = 5;
  static int audio_rate;
  static Uint16 audio_format;
  static int audio_channels;
  static int i;
  static int reverse_stereo = 0;
  static int reverse_sample = 0;

  using wav_list_t = std::vector<std::pair<std::string,Mix_Chunk*>>;
  using music_list_t = std::vector<std::pair<std::string,Mix_Music*>>;
  static wav_list_t gunshot_list;
  static music_list_t music_list;
  Mix_Chunk* mp5_shot = nullptr;
  Mix_Chunk* mg_shot = nullptr;
  static constexpr std::size_t SPAS_MAX = 3;
  static constexpr std::size_t SPAS_CYCLE_MAX = 1;
  static constexpr std::size_t P226_MAX = 2;
  static std::array<Mix_Chunk*,P226_MAX> p226_shots;
  static constexpr std::size_t MP5_MAX = 5;
  std::array<Mix_Chunk*,MP5_MAX> mp5_shots;
  std::array<Mix_Chunk*,SPAS_MAX> spas12_shots;
  std::array<Mix_Chunk*,SPAS_CYCLE_MAX> spas12_cycle;
  std::size_t mp5_index = 0;
  std::size_t load_gunshots(){
    m_debug("load_gunshots");
    gunshot_list.clear();
    sound::dir::load_folder(constants::gunshot_dir,&gunshot_list);
    mp5_shot = nullptr;
    std::string mp5 = constants::mp5_gunshot_wave;
    mp5 += ".wav";
    for(size_t i=0; i < SPAS_MAX;i++){
      spas12_shots[i] = nullptr;
    }
    for(size_t i=0; i < SPAS_CYCLE_MAX;i++){
      spas12_cycle[i] = nullptr;
    }
    for(const auto& p : gunshot_list){
      if(p.first.compare(mp5.c_str()) == 0){
        m_debug("mp5_gunshot_wave found");
        mp5_shot = p.second;
      }
      if(p.first.compare("p0.wav") == 0){
        mp5_shots[0] = p.second;
      }
      if(p.first.compare("p1.wav") == 0){
        mp5_shots[1] = p.second;
      }
      if(p.first.compare("p2.wav") == 0){
        mp5_shots[2] = p.second;
      }
      if(p.first.compare("p3.wav") == 0){
        mp5_shots[3] = p.second;
      }
      if(p.first.compare("p3.wav") == 0){
        mp5_shots[4] = p.second;
      }
      if(p.first.compare("p226-0.wav") == 0){
        p226_shots[0] = p.second;
      }
      if(p.first.compare("p226-1.wav") == 0){
        p226_shots[1] = p.second;
      }
      if(p.first.compare("spas12-0.wav") == 0){
        spas12_shots[0] = p.second;
      }
      if(p.first.compare("spas12-1.wav") == 0){
        spas12_shots[1] = p.second;
      }
      if(p.first.compare("spas12-2.wav") == 0){
        spas12_shots[2] = p.second;
      }
      if(p.first.compare("spas12-cycle-0.wav") == 0){
        spas12_cycle[0] = p.second;
      }
      if(p.first.compare("mg.wav") == 0){
        mg_shot = p.second;
      }

      Mix_VolumeChunk(p.second,50);
    }
    return gunshot_list.size();
  }
  std::size_t load_music(){
    std::size_t loaded_okay = 0;
    for(size_t i=0; constants::music_list[i]; i++){
      std::string path = constants::music_dir;
      path += constants::music_list[i];
      path += ".wav";
      auto created = Mix_LoadMUS(path.c_str());
      if(created == nullptr){
        std::cerr << "[ERROR]: cannot load track: '" << path << "'\n";
        continue;
      }
      music_list.emplace_back(constants::music_list[i],created);
      ++loaded_okay;
    }
    return loaded_okay;
  }
  static constexpr std::size_t CHANNEL_MAX = 16;
  void init(){
    m_debug("[init()]: initializing sound assets");
    /* Open the audio device */
    audio_rate = MIX_DEFAULT_FREQUENCY;
    audio_format = MIX_DEFAULT_FORMAT;
    audio_channels = 2; /* 2 = stereo, 1 = mono, default =  MIX_DEFAULT_CHANNELS */
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, 4096) < 0) {
      m_error("Couldn't open audio: " << SDL_GetError());
      return;
    } else {
      Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    }
    Mix_AllocateChannels(CHANNEL_MAX);

    m_debug(load_gunshots() << " wav gunshot files loaded");
    m_debug(load_music() << " wav MUSIC files loaded");
  }
  int start_track(const std::string& track_name){
#ifdef NO_MUSIC 
    return 0;
#else
    for(const auto& p : music_list){
      if(p.first.compare(track_name.data()) == 0){
        std::cout << "[MUSIC]: now playing: '" << p.first << "'\n";
        Mix_PlayMusic(p.second,-1);
        return 0;
      }
    }
#endif
    return -1;
  }
  void pause_music(){
    Mix_PauseMusic();
  }
  void resume_music(){
    Mix_ResumeMusic();
  }
  static size_t wave_list_index = 0;
  static size_t channel_index = 0;
  void stop_mp5_gunshot(){
    Mix_HaltChannel(GUNSHOT_AUDIO_CHANNEL);
  }
  void play_mg_gunshot(){
    Mix_PlayChannel(GUNSHOT_AUDIO_CHANNEL,mg_shot,0);
  }
  void play_mp5_gunshot(){
    //if(Mix_PlayChannelTimed(GUNSHOT_AUDIO_CHANNEL,mp5_shot,0,220) == -1){
    //  m_error("Mix_PlayChannel failed with: " << Mix_GetError());
    //}
    Mix_PlayChannel(GUNSHOT_AUDIO_CHANNEL,mp5_shots[rand_between(0,MP5_MAX - 1)],0);
  }
  void play_p226_gunshot(){
    Mix_PlayChannel(GUNSHOT_AUDIO_CHANNEL,p226_shots[rand_between(0,P226_MAX - 1)],0);
  }
  void program_exit(){
    for(const auto& music : music_list){
      Mix_FreeMusic(music.second);
    }
    for(const auto& pair : gunshot_list){
      Mix_FreeChunk(pair.second);
    }
  }
  void play_spas12_gunshot(){
    Mix_PlayChannel(SPAS12_CHANNEL,spas12_shots[rand_between(0,SPAS_MAX- 1)],0);
  }
  void play_spas12_cycle(){
    m_debug("play_spas12_cycle");
    //Mix_PlayChannel(CYCLE_CHANNEL,spas12_cycle[rand_between(0,SPAS_CYCLE_MAX- 1)],0);
    Mix_PlayChannel(CYCLE_CHANNEL,spas12_cycle[0],0);
  }
  void play_weapon(uint32_t w){
    if(is_pistol(w)){
      play_p226_gunshot();
      return;
    }
    if(is_smg(w)){
      play_mp5_gunshot();
      return;
    }
    if(is_shotgun(w)){
      play_spas12_gunshot();
      return;
    }
  }
};
