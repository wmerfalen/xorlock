#ifndef __ABILITIES_DRONE_HEADER__
#define __ABILITIES_DRONE_HEADER__
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include "../actor.hpp"
#include "../world.hpp"
#include "../coordinates.hpp"
#include "../bullet-pool.hpp"
#include "../debug.hpp"
#include "../draw.hpp"

#include "../behaviour-tree.hpp"
#include "../npc-id.hpp"
#include "../npc/paths.hpp"
#include "../weapons.hpp"
#include "../constants.hpp"
#include "../weapons/primary.hpp"
#include "../player.hpp"

namespace abilities::drone {
  void init();
  void tick();
  void space_bar_pressed();
  void program_exit();
  struct Drone {
    enum state_t : uint16_t {
      BOOT_UP,
      ASCEND_START,
      ASCEND_MID,
      ASCEND_END,
      LOITER,
      SELF_DESTRUCT,
    };
    int x;
    int y;
    int movement_amount;
    int cx;
    int cy;
    int hp;
    int max_hp;
    int target_angle;
    int angle;
    uint8_t opacity;
    uint8_t charge;
    bool ready;
    uint64_t move_at;
    uint32_t call_counter;
    uint64_t done_at;
    int8_t loiter_wave;
    std::vector<Asset*> states;
    std::size_t state_index;
    Actor self;
    state_t state;
    Drone(const Drone& copy) = delete;
    Drone();
    ~Drone();
    void tick();
    void calc();
    bool draw_lines();
  };
};
#endif
