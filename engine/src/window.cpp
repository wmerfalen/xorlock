#include "window.hpp"

namespace window::resolution {
  std::array<modes_t,4> modes = {
    modes_t{1024,1024,"1024x1024",1.0,13},
    modes_t{1536,1024,"1024x1024",1.5,15},
    modes_t{2048,1024,"2048x1024",2.0,16},
    modes_t{1024,1024,"1024x1024"},
  };
};
int WIN_WIDTH = 1024 * 1.5;
int WIN_HEIGHT = 1024;
