#ifndef __SOUND_RELOAD_HEADER__
#define __SOUND_RELOAD_HEADER__
#include <SDL2/SDL.h>
#include "../constants.hpp"

namespace sound::reload {
  void play_eject();
  void play_pull_replacement_mag();
  void play_load_mag();
  void play_weapon_slide();
  void stop_mp5_reload();
  void init();
  void load_reload();
  void program_exit();
};

#endif
