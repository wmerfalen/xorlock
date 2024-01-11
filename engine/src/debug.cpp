#include "debug.hpp"
#include <SDL2/SDL.h>


namespace dbg {

	std::string dump(SDL_Rect* r) {
		std::string s = "x: ";
		s += std::to_string(r->x) + "\n";
		s += "y: " + std::to_string(r->y) + "\n";
		s += "w: " + std::to_string(r->w) + "\n";
		s += "h: " + std::to_string(r->h) + "\n";
		return s;
	}
  bool use_primary_stat_editor = false;
  bool use_test_drops = false;
  bool use_unlimited_abilities = false;

  bool primary_stat_editor(){
    return use_primary_stat_editor;
  }
  void set_primary_stat_editor(bool b){
    use_primary_stat_editor = b;
  }
  void set_test_drops(bool b){
    use_test_drops = b;
  }
  void set_defaults(){
    set_primary_stat_editor(false);
    set_test_drops(false);
    set_unlimited_abilities(false);
  }
  bool test_drops(){
    return use_test_drops;
  }
  bool unlimited_abilities(){
    return use_unlimited_abilities;
  }
  void set_unlimited_abilities(bool b){
    use_unlimited_abilities = b;
  }
};
