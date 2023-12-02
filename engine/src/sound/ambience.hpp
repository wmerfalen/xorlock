#ifndef __SOUND_AMBIENCE_HEADER__
#define __SOUND_AMBIENCE_HEADER__
#include <SDL2/SDL.h>
#include "../constants.hpp"

namespace sound::ambience {
  void init();
  std::size_t reload_ambiences();
  void play_random_mix();
  void pause_music();
  void resume_music();
  void stir_up_the_pot();
  void program_exit();
};

#endif
