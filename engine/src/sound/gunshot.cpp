#include "gunshot.hpp"
#include "./reload.hpp"
#include <pthread.h>
#include <mutex>
#include "../tick.hpp"
#include <SDL2/SDL_mixer.h>
#include <sys/types.h>
#include <dirent.h>

#define m_debug(A) std::cout << "[SOUND][GUNSHOT][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SOUND][GUNSHOT][ERROR]: " << A << "\n";
namespace sound {
  static constexpr int GUNSHOT_AUDIO_CHANNEL = 0;
  static constexpr int RELOAD_AUDIO_CHANNEL = 0;
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
  static Mix_Chunk* mp5_shot = nullptr;
  std::size_t load_folder(const std::string& dir_name, wav_list_t* storage ){
    std::size_t loaded_ok = 0;
    m_debug("load_folder: " << dir_name);
    DIR * fp = opendir(dir_name.c_str());
    if(!fp){
      m_error("UNABLE to open '" << dir_name << "' directory");
      return loaded_ok;
    }
    struct dirent * dp = nullptr;
    while((dp = readdir(fp)) != nullptr){
      std::string s = dp->d_name;
      m_debug("checking: '" << s << "'");
      if(s.find(".wav") == std::string::npos){
        continue;
      }
      std::string tmp = dir_name;
      if(tmp[tmp.length()-1] != '/'){
        tmp += "/";
      }
      tmp += s;
      auto created = Mix_LoadWAV(tmp.c_str());
      if(created == nullptr){
        m_error("couldnt open: '" << tmp << "'");
        continue;
      }
      m_debug("loading wav: '" << tmp << "'");
      storage->emplace_back(s,created);
      ++loaded_ok;
    }
    return loaded_ok;
  }
  static constexpr std::size_t MP5_MAX = 5;
  std::array<Mix_Chunk*,MP5_MAX> mp5_shots;
  std::size_t mp5_index = 0;
  std::size_t load_gunshots(){
    m_debug("load_gunshots");
    gunshot_list.clear();
    auto loaded_ok = load_folder(constants::gunshot_dir,&gunshot_list);
    mp5_shot = nullptr;
    std::string mp5 = constants::mp5_gunshot_wave;
    mp5 += ".wav";
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
    }
    return loaded_ok;
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
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, 4096) < 0) {
      m_error("Couldn't open audio: " << SDL_GetError());
      return;
    } else {
      Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    }
    Mix_AllocateChannels(CHANNEL_MAX);

    m_debug(load_gunshots() << " wav gunshot files loaded");
    m_debug(load_music() << " wav MUSIC files loaded");
    /* Initialize variables */
    audio_rate = MIX_DEFAULT_FREQUENCY;
    audio_format = MIX_DEFAULT_FORMAT;
    audio_channels = MIX_DEFAULT_CHANNELS;
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
  static size_t wave_list_index = 0;
  static size_t channel_index = 0;
  void stop_mp5_gunshot(){
    Mix_HaltChannel(GUNSHOT_AUDIO_CHANNEL);
  }
  void play_mp5_gunshot(){
    if(Mix_PlayChannelTimed(/*channel_index*/GUNSHOT_AUDIO_CHANNEL,mp5_shot,0,220) == -1){
      m_error("Mix_PlayChannel failed with: " << Mix_GetError());
    }
  }
};
