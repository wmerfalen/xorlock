#include "ability.hpp"
#include "asset.hpp"
#include <map>
#include <set>
#include <optional>
#include <functional>
#include "rng.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[ABILITY][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[ABILITY][ERROR]: " << A << "\n";

namespace ability {
  static constexpr std::size_t ABILITY_MAX = 10;
  static std::array<Ability,ABILITY_MAX> list;
  enum ability_beta_t : uint32_t {
    AB_WIELD_DUAL_PISTOLS, // TODO:
    AB_AIRSTRIKE, // Implemented
    AB_CLOSE_AIR_SUPPORT, // TODO: 
  };
  enum demo_ability_t : uint32_t {

  };
  void init(){

  }
  void tick(){

  }
  void program_exit(){

  }
};
