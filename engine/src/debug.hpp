#ifndef __DEBUG_HEADER__
#define __DEBUG_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>

#ifdef LD
#undef LD
#endif

#define LD(A) std::cout << "[DEBUG]:" << __LINE__ << ": " << A << "\n";
#define DUMP_LINE() std::cout << __FILE__ << ":" << __LINE__ << "(" << __FUNCTION__ << ")\n";

namespace dbg {
	std::string dump(SDL_Rect* r);
  void set_defaults();
  bool primary_stat_editor();
  void set_primary_stat_editor(bool b);
  bool test_drops();
  void set_test_drops(bool b);
  bool unlimited_abilities();
  void set_unlimited_abilities(bool b);
  bool unlimited_ammo();
  void set_unlimited_ammo(bool);
};
#endif
