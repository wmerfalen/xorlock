#include "gunshot.hpp"
#include <pthread.h>
#include <mutex>
#include "../tick.hpp"
//#include "SDL/SDL.h"
#include <SDL2/SDL_mixer.h>

#define m_debug(A) std::cerr << "[DEBUG]:gunshot.cpp: " << A << "\n";
namespace sound {
  gunshot_sound::gunshot_sound(const std::string& in_file_name) : wave_loaded(false), file_name(in_file_name) {
    if(load() < 0){
      std::cerr << "[gunshot_sound() constructor]: FAILED to load wav: '" << in_file_name << "'\n";
    }
  }
  gunshot_sound::~gunshot_sound() {
    if(wave_loaded){
      SDL_FreeWAV(wav_buffer);
    }
  }
  int gunshot_sound::load(){
    if(SDL_LoadWAV(file_name.c_str(), &wav_spec, &wav_buffer, &wav_length) == nullptr){
      std::cerr << "[SDL_LoadWAV][ERROR]: Couldn't open wav: " << SDL_GetError() << "\n";
      wave_loaded = false;
      return -1;
    }
    device_id = SDL_OpenAudioDevice(nullptr, 0, &wav_spec, nullptr, 0);
    wave_loaded = true;
    return 0;
  }
  void gunshot_sound::play(){
    // play audio
    SDL_QueueAudio(device_id, wav_buffer, wav_length);
    SDL_PauseAudioDevice(device_id, 0);
  }
  std::unique_ptr<gunshot_sound> mp5_gunshot = nullptr;
  void stop_mp5_gunshot(){
  }
  static int still_playing(void)
  {
    return(Mix_Playing(0));
  }
  static int audio_rate;
  static Uint16 audio_format;
  static int audio_channels;
  static int i;
  static int reverse_stereo = 0;
  static int reverse_sample = 0;

  static std::vector<std::pair<std::string,Mix_Chunk*>> wave_list;
  static std::vector<std::pair<std::string,Mix_Music*>> music_list;
  static Mix_Chunk* mp5_shot = nullptr;
  void init(){
    std::cout << "[SOUND][GUNSHOT][init()]: initializing sound assets\n";
    //mp5_gunshot = std::make_unique<gunshot_sound>("../assets/sound/gunshot-high-p0.wav");
    /* Open the audio device */
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, 4096) < 0) {
      printf("Couldn't open audio: %s\n", SDL_GetError());
      return;
    } else {
      Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
      printf("Opened audio at %d Hz %d bit%s %s", audio_rate,
          (audio_format&0xFF),
          (" (float)"),
          (audio_channels > 2) ? "surround" :
          (audio_channels > 1) ? "stereo" : "mono");
    }

    /* Load the requested wave file */
    for(size_t i=0; constants::wave_list[i]; i++){
      std::string path = "../assets/sound/";
      path += constants::wave_list[i];
      path += ".wav";
      auto created = Mix_LoadWAV(path.c_str());
      if(created == nullptr){
        std::cerr << "[ERROR]: cannot load wav: '" << path << "'\n";
        continue;
      }
      wave_list.emplace_back(constants::wave_list[i],created);
    }
    if(mp5_shot == nullptr){
      for(const auto& p : wave_list){
        if(p.first.compare(constants::mp5_gunshot_wave) == 0){
          mp5_shot = p.second;
          break;
        }
      }
    }
    for(size_t i=0; constants::music_list[i]; i++){
      std::string path = "../assets/sound/";
      path += constants::music_list[i];
      path += ".wav";
      auto created = Mix_LoadMUS(path.c_str());
      if(created == nullptr){
        std::cerr << "[ERROR]: cannot load track: '" << path << "'\n";
        continue;
      }
      music_list.emplace_back(constants::music_list[i],created);
    }
    std::cout << "[STATUS][SOUND]: " << wave_list.size() << " wav SFX files loaded\n";
    std::cout << "[STATUS][SOUND]: " << music_list.size() << " wav MUSIC files loaded\n";
    /* Initialize variables */
    audio_rate = MIX_DEFAULT_FREQUENCY;
    audio_format = MIX_DEFAULT_FORMAT;
    audio_channels = MIX_DEFAULT_CHANNELS;

    std::cout << "audio_channels: " << audio_channels << "\n";
    std::cout << "audio_format: " << audio_format << "\n";
    std::cout << "audio_rate: " << audio_rate << "\n";

  }
  int start_track(const std::string& track_name){
    for(const auto& p : music_list){
      if(p.first.compare(track_name.data()) == 0){
        std::cout << "[MUSIC]: now playing: '" << p.first << "'\n";
        Mix_PlayMusic(p.second,-1);
        return 0;
      }
    }
    return -1;
  }
  void play_mp5_gunshot(){
    Mix_PlayChannel(2, mp5_shot, 0);
  }
};
