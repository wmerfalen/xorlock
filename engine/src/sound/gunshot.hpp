#ifndef __SOUND_GUNSHOT_HEADER__
#define __SOUND_GUNSHOT_HEADER__
#include <SDL2/SDL.h>
#include "../constants.hpp"

namespace sound {
  void init();
  void play_mp5_gunshot();
  void stop_mp5_gunshot();
  int start_track(const std::string& track_name);
  void reload_gunshots();
  void pause_music();
  void resume_music();
};

#endif
