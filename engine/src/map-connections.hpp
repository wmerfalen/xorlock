#ifndef __MAP_CONNECTIONS_HEADER__
#define __MAP_CONNECTIONS_HEADER__

#include "actor.hpp"
#include "world.hpp"
#include "coordinates.hpp"
#include "viewport.hpp"
#include "circle.hpp"
#include "rng.hpp"
#include "draw.hpp"
#include <vector>
#include <functional>
#include <memory>
#include "direction.hpp"
#include <set>

namespace map_connections {
  struct Connection {
    std::vector<SDL_Rect> from;
    std::string destination_level;
    SDL_Rect destination;
  };
	void move_map(int direction,int amount);
	void tick();
	void init();
  void program_exit();
};

#endif
