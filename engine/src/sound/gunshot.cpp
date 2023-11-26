#include "gunshot.hpp"

#define m_debug(A) std::cerr << "[DEBUG]:BULLET.CPP: " << A << "\n";
//#define DRAW_VECTOR_BULLET_TRAIL
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
};
