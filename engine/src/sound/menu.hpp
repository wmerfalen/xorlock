#ifndef __SOUND_MENU_HEADER__
#define __SOUND_MENU_HEADER__
#include <SDL2/SDL.h>
#include "../constants.hpp"

namespace sound::menu {
  void init();
  void play_menu_change();
  void play_menu_select_item();
  std::size_t reload_menus();
};

#endif
