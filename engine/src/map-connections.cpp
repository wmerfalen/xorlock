#include "map-connections.hpp"
#include "wall.hpp"
#include "map.hpp"
#include "world.hpp"
#include "player.hpp"
#include <functional>

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[MAP-CONNECTIONS][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[MAP-CONNECTIONS][ERROR]: " << A << "\n";
namespace map_connections {
  void tick() {
  }
  void init() {
    m_debug("init()");
  }
  void program_exit(){
  }
};
