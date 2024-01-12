#ifndef __WORLD_HEADER__
#define __WORLD_HEADER__
#include <forward_list>
#include "actor.hpp"
#include "hallway.hpp"

#include "viewport.hpp"
#include <map>
#include <set>

// TODO: determine if these need to change depending on window resolution
static constexpr int CELL_WIDTH = 65;
static constexpr int CELL_HEIGHT = 65;
struct World {
	uint64_t width;
	uint64_t height;
	int x;
	int y;

  bool is_npc(Actor* a);
  void register_npc(Actor* a);
  void unregister_npc(Actor* a);
  void register_actor(Actor* a);
  void unregister_actor(Actor* a);
  void register_points(double* , double *);
  void unregister_points(double* , double*);
  template <typename TContainer>
  inline void erase_actors(const TContainer& storage){
    for(const auto& actor : storage){
      unregister_npc(actor);
      unregister_actor(actor);
    }
  }
  std::vector<Actor*>& actors() { return m_actors; }
  std::vector<std::pair<double*,double*>>& points(){ return m_points; }
  std::vector<Actor*>& npcs(){ return m_npc; }
  int start_tile_x();
  int start_tile_y();
  private:
  std::vector<Actor*> m_actors;
  std::vector<std::pair<double*,double*>> m_points;
  std::vector<Actor*> m_npc;
};
extern void register_actor(Actor* a);
extern void unregister_actor(Actor* a);
extern void register_xy(double* _x, double* _y);
extern void unregister_xy(double* _x, double* _y);
void cleanup_dead_npcs(const std::vector<Actor*>& corpses);

extern std::unique_ptr<World> world;

void move_map_by(int dir, int amount);

void init_world(const std::string& level);


int import_tiled_world(const std::string&  _world_csv);

void world_tick();

void world_program_exit();

#endif
