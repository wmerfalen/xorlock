#ifndef __SOUND_NPC_HEADER__
#define __SOUND_NPC_HEADER__
#include <SDL2/SDL.h>
#include "../constants.hpp"

namespace sound::npc {
  void play_eject();
  void play_pull_replacement_mag();
  void play_load_mag();
  void play_weapon_slide();
  void init();
  void load_sounds();
  void play_npc_pain(const int& npc_type);
  void play_death_sound(const int& npc_type);
  void play_corpse_sound(const int& npc_type,const int& hp);
  void program_exit();
};

#endif
