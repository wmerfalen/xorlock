#ifndef __SOUND_GUNSHOT_HEADER__
#define __SOUND_GUNSHOT_HEADER__
#include <SDL2/SDL.h>
#include "../constants.hpp"

#define m_debug(A) std::cerr << "[DEBUG]:BULLET.CPP: " << A << "\n";
//#define DRAW_VECTOR_BULLET_TRAIL
namespace sound {
  struct gunshot_sound {
    gunshot_sound() = delete;
    gunshot_sound(const std::string& in_file_name);
    ~gunshot_sound();
    int load();
    void play();
    private:
    SDL_AudioSpec wav_spec;
    uint32_t wav_length;
    uint8_t * wav_buffer;
    bool wave_loaded;
    std::string file_name;
    SDL_AudioDeviceID device_id;
  };
  void init();
  void play_mp5_gunshot();
  void stop_mp5_gunshot();
  int start_track(const std::string& track_name);
};

#endif
