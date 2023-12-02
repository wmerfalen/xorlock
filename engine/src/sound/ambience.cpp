#include "ambience.hpp"
#include "directory.hpp"
#include <pthread.h>
#include <mutex>
#include "../tick.hpp"
#include "../rng.hpp"
#include "./wave-length.hpp"
#include <SDL2/SDL_mixer.h>
#include <sys/types.h>
#include <dirent.h>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[SOUND][GUNSHOT][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SOUND][GUNSHOT][ERROR]: " << A << "\n";
namespace sound::ambience {
  static constexpr int AUDIO_CHANNEL = 2;
  static constexpr int AUDIO_CHANNEL_START = 2;
  static constexpr int AUDIO_CHANNEL_END = 10;

  using wav_list_t = std::vector<std::pair<std::string,Mix_Chunk*>>;
  static wav_list_t ambience_list;
  static constexpr std::size_t MAX_AMBIENCE_PLAYING = 8;
  struct wav_data_t {
    Mix_Chunk* chunk;
    int loops;
    float seconds;
    std::string path;
    bool play_once;
  };
  struct playback_data_t {
    Mix_Chunk* chunk;
    int loops;
    float seconds;
    uint64_t start_tick;
    bool play_once;
    bool active;
    std::string path;
  };
  std::array<playback_data_t,MAX_AMBIENCE_PLAYING> playing_now;
  std::vector<std::string> play_once;

  static std::vector<wav_data_t> ambient_library;

  float get_seconds(Mix_Chunk* chunk){
    for(const auto& entry : ambient_library){
      if(entry.chunk == chunk){
        return entry.seconds;
      }
    }
    return -1;
  }
  std::size_t reload_ambiences(){
    m_debug("reload_ambiences");
    ambience_list.clear();
    play_once.clear();
    ambient_library.clear();
    for(size_t i=0; i < MAX_AMBIENCE_PLAYING;i++){
      playing_now[i].active = 0;
    }
    sound::dir::load_folder(constants::ambience_dir,&ambience_list);
    for(const auto& pair : ambience_list){
      if(pair.first.find("radio-chatter") != std::string::npos){
        play_once.emplace_back(pair.first);
      }
      std::string file = constants::ambience_dir;
      file += pair.first;
      for(const auto& wl_pair : sound::wave_length::list){
        if(wl_pair.first.compare(file.c_str()) == 0){
          m_debug("found ambient wave_length entry: '" << wl_pair.first << "': " << wl_pair.second);
          if(pair.first.find("radio-chatter") != std::string::npos){
            ambient_library.emplace_back(pair.second,0,wl_pair.second,file,true);
          }else{
            ambient_library.emplace_back(pair.second,0,wl_pair.second,file,false);
          }
        }
      }
    }
    return ambience_list.size();
  }
  void init(){
    m_debug("[init()]: initializing sound assets");
    /* Open the audio device */
    m_debug(reload_ambiences() << " wav ambience files loaded");
    for(std::size_t i=0; i < MAX_AMBIENCE_PLAYING;i++){
      playing_now[i].active = 0;
    }
  }
  void stir_up_the_pot(){
    m_debug("stir_up_the_pot");
    std::size_t i = 0;
    for(const auto& pair: ambience_list){
      if(rng::chance(33)){
        playing_now[i].active = 1;
        playing_now[i].chunk = pair.second;
        playing_now[i].loops = 0;
        playing_now[i].seconds = get_seconds(pair.second);
        //playing_now[i].start_tick = tick::get();
        playing_now[i].path = pair.first;
        m_debug("queing up: '" << pair.first << "'");
      }else{
        playing_now[i].active = 0;
      }
      ++i;
    }
  }
  std::string get_file_name(Mix_Chunk* c){
    for(const auto& pair : ambience_list){
      if(c == pair.second){
        return pair.first;
      }
    }
    return "<unknown>";
  }
  void play_random_mix(){
    int audio_channel = AUDIO_CHANNEL_START;
    for(auto& chunk : playing_now){
      if(chunk.active){
        if(chunk.play_once){
          chunk.loops = 0;
          m_debug("playing ambient file: '" << chunk.path << "' once");
        }else{
          chunk.loops = rand_between(3,20);
          m_debug("playing ambient file: '" << chunk.path << "' " << chunk.loops << " times");
        }
        chunk.start_tick = tick::get();
        Mix_PlayChannel(audio_channel,chunk.chunk,chunk.loops);
        Mix_VolumeChunk(chunk.chunk,rand_between(13,33));
        ++audio_channel;
        if(audio_channel > AUDIO_CHANNEL_END){
          audio_channel = 0;
        }
      }
    }
  }
};
