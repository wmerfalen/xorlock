#include <forward_list>
#include "actor.hpp"
#include "player.hpp"
#include "world.hpp"
#include "map.hpp"
#include "colors.hpp"
#include "wall.hpp"
#include "direction.hpp"
#include <unordered_map>

#include "npc-spetsnaz.hpp"
#include "tiled/parser.hpp"

extern std::vector<SDL_Surface*> surface_list;
extern std::vector<SDL_Texture*> texture_list;
bool World::is_npc(Actor* a){
  return std::find(m_npc.cbegin(),m_npc.cend(),a) != m_npc.cend();
}
void World::unregister_npc(Actor* a){
  std::erase_if(m_npc,[&](Actor* stored) -> bool {
      return stored == a;
      });
  std::erase_if(m_actors,[&](Actor* stored) -> bool {
      return stored == a;
      });
}
void World::register_npc(Actor* a){
  m_npc.emplace_back(a);
  m_actors.emplace_back(a);
}
#ifdef WORLD_DEBUG
#define m_debug(A) std::cerr << "[DEBUG]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";
#else
#define m_debug(A)
#endif
#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[WORLD][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[WORLD][ERROR]: " << A << "\n";
enum nb : uint8_t {
  NW = 0,
  N,
  NE,
  E,
  SE,
  S,
  SW,
  W,
};

//static int hall_width = 80;
static int hall_height = 80;
static SDL_Rect top = {0,0,win_width(),hall_height};
extern int32_t START_X;
extern int32_t START_Y;
bool top_intersects_with(Player& p) {
  return SDL_HasIntersection(&top,&p.self.rect);
}
std::unordered_map<wall::Wall*,std::vector<wall::Wall*>> cached_walkable;
std::size_t walkable_cache_count = 0, walkable_looped_count = 0;
std::unordered_map<wall::Wall*,std::array<wall::Wall*,8>> surrounding_walls;
std::size_t initial_load_count = 0,total_call_count = 0, cached_call_count = 0, cache_missed_count = 0;
std::unordered_map<wall::Wall*,bool> cached_is_walkable;
std::size_t cached_is_walkable_count = 0, is_walkable_call_count = 0, is_walkable_load_count = 0;
std::size_t walkable_neighbors_call_count = 0; // corresponds to walkable(std::array,dirs)

void report_world(){
  std::cout << "walkable_cache_count: " << walkable_cache_count << "\n";
  std::cout << "walkable_looped_count: " << walkable_looped_count << "\n";
  std::cout << "cached_is_walkable_count: " << cached_is_walkable_count << "\n";
  std::cout << "is_walkable_call_count: " << is_walkable_call_count << "\n";
  std::cout << "is_walkable_load_count: " << is_walkable_load_count << "\n";
  std::cout << "walkable_neighbors_call_count: " << walkable_neighbors_call_count << "\n";
}
void World::register_points(double* _x,double* _y){
  m_points.emplace_back(_x,_y);
}
void World::unregister_points(double* _x,double* _y){
  std::erase_if(m_points, [&](auto& pair) -> bool {
      return pair.first == _x && pair.second == _y;
      });
}
void register_xy(double* _x, double* _y){
  world->register_points(_x,_y);
}
void unregister_xy(double* _x, double* _y){
  world->unregister_points(_x,_y);
}
void register_actor(Actor* a){
  world->register_actor(a);
}
void World::register_actor(Actor* a){
  m_actors.emplace_back(a);
}
void World::unregister_actor(Actor* a){
  std::erase_if(m_actors, [&](Actor* ac) -> bool {
      return ac == a;
      });
}
void unregister_actor(Actor* a){
  world->unregister_actor(a);
}
std::vector<wall::Wall*> get_walkable_toward(const Direction& dir,wall::Wall* from) {
  std::vector<wall::Wall*> walkable;
  std::size_t iteration = 1;
  std::size_t size_of = wall::walls.size();
  std::size_t wall_ctr = 0;
  if(cached_walkable.find(from) != cached_walkable.cend()){
    ++walkable_cache_count;
    return cached_walkable[from];
  }
  ++walkable_looped_count;
  while(wall_ctr < size_of) {
    wall_ctr = 0;
    for(const auto& w : wall::walls) {
      ++wall_ctr;
      if(dir == NORTH) {
        if(w->self.rect.y == from->self.rect.y - (CELL_HEIGHT * iteration) && w->self.rect.x == from->self.rect.x) {
          if(!w->walkable) {
            cached_walkable[from] = walkable;
            return walkable;
          }
          walkable.emplace_back(w.get());
          ++iteration;
          break;
        }
      } else if(dir == SOUTH) {
        if(w->self.rect.y == from->self.rect.y + (CELL_HEIGHT * iteration) && w->self.rect.x == from->self.rect.x) {
          if(!w->walkable) {
            cached_walkable[from] = walkable;
            return walkable;
          }
          walkable.emplace_back(w.get());
          ++iteration;
          break;
        }
      } else if(dir == EAST) {
        if(w->self.rect.x == from->self.rect.x + (CELL_WIDTH * iteration) && w->self.rect.y == from->self.rect.y) {
          if(!w->walkable) {
            cached_walkable[from] = walkable;
            return walkable;
          }
          walkable.emplace_back(w.get());
          ++iteration;
          break;
        }
      } else if(dir == WEST) {
        if(w->self.rect.x == from->self.rect.x - (CELL_WIDTH * iteration) && w->self.rect.y == from->self.rect.y) {
          if(!w->walkable) {
            cached_walkable[from] = walkable;
            return walkable;
          }
          walkable.emplace_back(w.get());
          ++iteration;
          break;
        }
      }
    }
  }
  cached_walkable[from] = walkable;
  return walkable;
}
std::array<wall::Wall*,8> get_surrounding_walls(wall::Wall* from) {
  std::array<wall::Wall*,8> neighbors;
  std::size_t i=0;
  ++total_call_count;
  if(surrounding_walls.find(from) != surrounding_walls.cend()){
    ++cached_call_count;
    return surrounding_walls[from];
  }
  for(const auto& w : wall::walls) {
    /**
     * x-----x-----x-----x
     * |  NW |  N  |  NE |
     * x-----x-----x-----|
     * |  W  |     |  E  |
     * x-----x-----x-----|
     * |  SW |  S  |  SE |
     * x-----x-----x-----x
     */
    if((from->self.rect.x - from->self.rect.w == w->self.rect.x) &&
        (from->self.rect.y - from->self.rect.h == w->self.rect.y)) {
      /**
       * Found NW
       */
      ++i;
      neighbors[nb::NW] = w.get();
    } else if((from->self.rect.y - from->self.rect.h == w->self.rect.y) &&
        (from->self.rect.x == w->self.rect.x)) {
      /**
       * Found N
       */
      ++i;
      neighbors[nb::N] = w.get();
    } else if((from->self.rect.x + from->self.rect.w == w->self.rect.x) &&
        (from->self.rect.y - from->self.rect.h == w->self.rect.y)) {
      /**
       * Found NE
       */
      ++i;
      neighbors[nb::NE] = w.get();
    } else if((from->self.rect.x - from->self.rect.w == w->self.rect.x) &&
        (from->self.rect.y == w->self.rect.y)) {
      /**
       * Found W
       */

      ++i;
      neighbors[nb::W] = w.get();
    } else if((from->self.rect.x - from->self.rect.w == w->self.rect.x) &&
        (from->self.rect.y + from->self.rect.h == w->self.rect.y)) {
      /**
       * Found SW
       */
      ++i;
      neighbors[nb::SW] = w.get();
    } else if((from->self.rect.x == w->self.rect.x) &&
        (from->self.rect.y + from->self.rect.h == w->self.rect.y)) {
      /**
       * Found S
       */
      ++i;
      neighbors[nb::S] = w.get();
    } else if((from->self.rect.x + from->self.rect.w == w->self.rect.x) &&
        (from->self.rect.y + from->self.rect.h == w->self.rect.y)) {
      /**
       * Found SE
       */
      ++i;
      neighbors[nb::SE] = w.get();
    } else if((from->self.rect.x + from->self.rect.w == w->self.rect.x) &&
        (from->self.rect.y == w->self.rect.y)) {
      /**
       * Found E
       */
      ++i;
      neighbors[nb::E] = w.get();
    }


    if(i == 8) {
      ++initial_load_count;
      return surrounding_walls[from] = neighbors;
    }
  }
  ++cache_missed_count;
  return neighbors;
}

bool walkable(wall::Wall* w) {
  ++is_walkable_call_count;
  if(cached_is_walkable.find(w) != cached_is_walkable.cend()){
    ++cached_is_walkable_count;
    return cached_is_walkable[w];
  }
  ++is_walkable_load_count;
  cached_is_walkable[w] = w == nullptr ? false : w->any_of(wall::WALKABLE);
  return cached_is_walkable[w];
}
using txt_t = wall::Texture;
bool walkable(std::array<wall::Wall*,8>* nbrs,std::vector<nb>&& dirs) {
  ++walkable_neighbors_call_count;
  for(auto&& d : dirs) {
    if(!walkable((*nbrs)[d])) {
      return false;
    }
  }
  return true;
}
void find_edge_connections() {
  m_debug("find_edge_connections entry");
  std::vector<wall::Wall*> found;
  for(const auto& w : wall::gateways){
    for(const auto dir : {
        NORTH,SOUTH,EAST,WEST
        }) {
      found = get_walkable_toward(dir,w);
      for(auto& tile : found) {
        tile->draw_color = colors::red();
        ++tile->connections;
      }
    }
  }
}
void find_edges() {
  m_debug("find_edges entry");
  std::array<wall::Wall*,8> nbrs;
  for(auto& w : wall::walls) {
    nbrs = get_surrounding_walls(w.get());
    /**
     * Checks for basically this:
     *
     * ......................
     * +============AA.......
     * _____________BB.......
     * ......................
     *
     * Where AA and BB are corners and the x's are walkable areas.
     * This will mark the area to the east of both corners as a gateway
     * It also marks SE as a gateway as well
     */
    if(w->type == txt_t::BR_TR_CRNR && nbrs[nb::S]->type == txt_t::BR_BR_CRNR &&
        walkable(&nbrs, {NW,N,NE,E,SE})) {
      nbrs[nb::E]->is_gateway = true;
      nbrs[nb::SE]->is_gateway = true;
      continue;
    }
    /**
     * Checks for essentially....
     *
     * ...........................
     * .........AA================
     * .........BB================
     * ...........................
     *
     * Where AA and BB are corners. It marks the area to the west
     * as a gateway.
     *
     * Also marks SW as a gateway
     */
    if(w->type ==txt_t::BR_TL_CRNR && nbrs[nb::S]->type ==txt_t::BR_BL_CRNR &&
        walkable(&nbrs, {NE,N,NE,W,SW})) {
      nbrs[nb::W]->is_gateway = true;
      nbrs[nb::SW]->is_gateway = true;
      continue;
    }
    /**
     * Checks for a north-south span of walls, with a southern
     * escape.
     *
     * ............W
     * ............W
     * ............W
     * ............A===========
     * ..........................
     BLD_LWALL   -> W
     BLD_BL_CRNR -> A
     */
    if(w->type ==txt_t::BLD_BL_CRNR &&
        nbrs[nb::N]->type == txt_t::BLD_LWALL &&
        walkable(&nbrs, {W,NW,SE,S,SW})) {
      nbrs[nb::W]->is_gateway = true;
      nbrs[nb::SW]->is_gateway = true;
      nbrs[nb::S]->is_gateway = true;
      continue;
    }

    /**
     * Checks for a west-east span with a eastern escape
     *
     * ............
     * AAAAAAAAAA..
     * ............
     * BLD_TWALL = A
     */
    if(w->type ==txt_t::BLD_TWALL &&
        nbrs[nb::W]->type ==txt_t::BLD_TWALL &&
        walkable(&nbrs, {NW,N,NE,E,SE,S})) {
      nbrs[nb::E]->is_gateway = true;
      nbrs[nb::SE]->is_gateway = true;
      nbrs[nb::S]->is_gateway = true;
      continue;
    }
    /**
     * Checks for a east-west span with a western escape
     *
     * ............
     * ..AAAAAAAAAA
     * ............
     * BLD_TWALL = A
     */
    if(w->type ==txt_t::BLD_TWALL &&
        nbrs[nb::E]->type == txt_t::BLD_TWALL &&
        walkable(&nbrs, {NW,N,W,SW,S})) {
      nbrs[nb::W]->is_gateway = true;
      nbrs[nb::SW]->is_gateway = true;
      nbrs[nb::S]->is_gateway = true;
      continue;
    }
    /**
     * Checks for a top right corner where the
     * escape could be N,NW,NE
     *
     * ............
     * BBBBBBBBBA..
     * =========C..
     * =========C..
     *
     BLD_TR_CRNR = A
     BLD_TWALL = B,
     BLD_RWALL = C
     */
    if(w->type ==txt_t::BLD_TR_CRNR &&
        nbrs[nb::W]->type == txt_t::BLD_TWALL &&
        nbrs[nb::S]->type == txt_t::BLD_RWALL &&
        walkable(&nbrs, {NW,N,NE,E,SE})) {
      nbrs[nb::N]->is_gateway = true;
      nbrs[nb::NE]->is_gateway = true;
      nbrs[nb::E]->is_gateway = true;
      continue;
    }

    /**
     * Checks for a south-north wall with an escape at the north
     *
     * ..........
     * .....AA...
     * .....AA...
     * .....AA...
     * .....AA...
     *
     BLD_LWALL = A
     */
    if(w->type == txt_t::BLD_LWALL &&
        nbrs[nb::S]->type == txt_t::BLD_LWALL &&
        walkable(&nbrs, {N,NW,NE,W,E})) {
      nbrs[nb::N]->is_gateway = true;
      nbrs[nb::NE]->is_gateway = true;
      nbrs[nb::NW]->is_gateway = true;
      continue;
    }

    /**
     * Checks for wall from west to east where the escape is
     * to the east
     *
     * ..............
     * AAAAAAAAAA....
     * ..............
     *
     BLD_BWALL = A
     */
    if(w->type == txt_t::BLD_BWALL &&
        nbrs[nb::W]->type == txt_t::BLD_BWALL &&
        walkable(&nbrs, {N,NE,E,SE,S})) {
      nbrs[nb::E]->is_gateway = true;
      nbrs[nb::SE]->is_gateway = true;
      nbrs[nb::S]->is_gateway = true;
      continue;
    }
    /**
     * Checks for wall from east to west where the escape is
     * to the west
     *
     * ..............
     * ...AAAAAAAAAAA
     * ..............
     *
     BLD_BWALL = A
     */
    if(w->type == txt_t::BLD_BWALL &&
        nbrs[nb::E]->type == txt_t::BLD_BWALL &&
        walkable(&nbrs, {N,NW,W,SW,S})) {
      nbrs[nb::W]->is_gateway = true;
      nbrs[nb::SW]->is_gateway = true;
      nbrs[nb::S]->is_gateway = true;
      continue;
    }
    /**
     * Checks for a south-east corner with a wall going north
     * and a wall going west. The escape is to the south east
     *
     * |||B..........
     * CCCA..........
     * ..............
     *
     BLD_BR_CRNR = A
     BLD_RWALL = B
     BLD_BWALL = C
     */
    if(w->type == txt_t::BLD_BR_CRNR &&
        nbrs[nb::N]->type == txt_t::BLD_RWALL &&
        nbrs[nb::W]->type == txt_t::BLD_BWALL &&
        walkable(&nbrs, {NE,E,SE,S,SW})) {
      nbrs[nb::E]->is_gateway = true;
      nbrs[nb::SE]->is_gateway = true;
      nbrs[nb::S]->is_gateway = true;
      continue;
    }

    /**
     * Checks for a wall going north to south where
     * the escape is to the south
     *
     * ......AA......
     * ......AA......
     * ......AA......
     * ......AA......
     * ..............
     *
     BLD_RWALL = A
     */
    if(w->type == txt_t::BLD_RWALL &&
        nbrs[nb::N]->type == txt_t::BLD_RWALL &&
        walkable(&nbrs, {W,SW,S,SE,E})) {
      nbrs[nb::S]->is_gateway = true;
      nbrs[nb::SE]->is_gateway = true;
      nbrs[nb::SW]->is_gateway = true;
      continue;
    }

    /**
     * Checks for a single stub where the wall is just one unit
     * extending from north to south and the escape is south
     *
     * ......AA......
     * ..............
     *
     BLD_RWALL = A
     */
    if(w->type == txt_t::BLD_RWALL &&
        walkable(&nbrs, {W,SW,S,SE,E})) {
      nbrs[nb::S]->is_gateway = true;
      nbrs[nb::SE]->is_gateway = true;
      nbrs[nb::SW]->is_gateway = true;
      continue;
    }

    /**
     * Checks for a wall going south to north where
     * the escape is to the north
     *
     * ..............
     * ......AA......
     * ......AA......
     * ......AA......
     * ......AA......
     *
     BLD_RWALL = A
     */
    if(w->type == txt_t::BLD_RWALL &&
        nbrs[nb::S]->type == txt_t::BLD_RWALL &&
        walkable(&nbrs, {W,NW,N,NE,E})) {
      nbrs[nb::N]->is_gateway = true;
      nbrs[nb::NE]->is_gateway = true;
      nbrs[nb::NW]->is_gateway = true;
      continue;
    }

    /**
     * Checks for a single stub where the wall is just one unit
     * extended from south to north and where the escape is
     * to the north
     *
     * ..............
     * ......AA......
     *
     BLD_RWALL = A
     */
    if(w->type == txt_t::BLD_RWALL &&
        walkable(&nbrs, {W,NW,N,NE,E})) {
      nbrs[nb::N]->is_gateway = true;
      nbrs[nb::NE]->is_gateway = true;
      nbrs[nb::NW]->is_gateway = true;
      continue;
    }
    /**
     * Checks for north-west corner where the escape is west
     *
     * .....................
     * .......ABBBBBBBBBBBB
     * .......C============
     * .......C============
     BLD_TL_CRNR = A
     BLD_TWALL = B
     BLD_LWALL = C
     */
    if(w->type == txt_t::BLD_TL_CRNR &&
        nbrs[nb::E]->type == txt_t::BLD_TWALL &&
        nbrs[nb::S]->type == txt_t::BLD_LWALL &&
        walkable(&nbrs, {NW,N,NE,SW})) {
      nbrs[nb::N]->is_gateway = true;
      nbrs[nb::NW]->is_gateway = true;
      nbrs[nb::W]->is_gateway = true;
      continue;
    }
  }
}
std::size_t index_gateways() {
  std::size_t indexed = 0;
  wall::gateways.clear();
  for(const auto& w : wall::walls) {
    if(w->is_gateway) {
      wall::gateways.emplace_back(w.get());
      ++indexed;
    }
  }
  return indexed;
}
void tie_walls_together() {
  std::array<wall::Wall*,8> nbrs;
  for(auto& w : wall::walls) {
    std::fill(nbrs.begin(),nbrs.end(),nullptr);
    nbrs = get_surrounding_walls(w.get());
    w->north = nbrs[nb::N];
    w->north_east = nbrs[nb::NE];
    w->north_west = nbrs[nb::NW];
    w->south = nbrs[nb::S];
    w->south_east = nbrs[nb::SE];
    w->south_west = nbrs[nb::SW];
    w->east = nbrs[nb::E];
    w->west = nbrs[nb::W];
  }
}
int World::start_tile_x(){
  for(const auto& w : wall::walls){
    if(w->type == wall::START_TILE){
      return w->orig_rect.x;
    }
  }
  return 0;
}
int World::start_tile_y(){
  for(const auto& w : wall::walls){
    if(w->type == wall::START_TILE){
      return w->orig_rect.y;
    }
  }
  return 0;
}
void init_world(const std::string& level) {
  int status = import_tiled_world(level);
  std::cout << "import_tiled_world status: " << status << "\n";
  find_edges();
  find_edge_connections();
  auto gw_indexed = index_gateways();
  std::cout << gw_indexed << " gateways indexed\n";
  tie_walls_together();
  cached_walkable.clear();
  surrounding_walls.clear();
}
int column_count = 0;

int import_tiled_world(const std::string& _world_csv) {
  static constexpr int READ_BUFFER_SIZE = 2048;
  std::array<char,READ_BUFFER_SIZE> read_buffer;
  FILE* fp = fopen(_world_csv.c_str(),"r");
  if(!fp) {
    return -1;
  }
  std::fill(read_buffer.begin(),read_buffer.end(),0);
  std::vector<std::vector<int16_t>> map;

  while(!feof(fp) && fgets(&read_buffer[0],READ_BUFFER_SIZE - 1,fp)) {
    std::string c;
    int element = 0;
    std::vector<int16_t> row;
    for(const auto& ch : read_buffer) {
      if(ch == '\0') {
        if(c.length()) {
          element = atoi(c.data());
          row.emplace_back(element);
        }
        break;
      }
      if(ch == '\n' || ch == ',') {
        element = atoi(c.data());
        row.emplace_back(element);
        c.clear();
        continue;
      }
      if(isdigit(ch) || ch == '-') {
        c += ch;
      }
    }
    map.emplace_back(row);
    std::fill(read_buffer.begin(),read_buffer.end(),0);
  }
  if(map.size()){
    column_count = map[0].size();
  }
  fclose(fp);
  for(std::size_t row = 0; row < map.size(); row++) {
    std::size_t col = 0;
    for(const auto& val : map[row]) {
      wall::draw_wall_at(/*START_X +*/ (col * CELL_HEIGHT), /*START_Y +*/ (row * CELL_WIDTH), CELL_WIDTH,CELL_HEIGHT,static_cast<wall::Texture>(val));
      ++col;
    }
    col = 0;
  }

  return 0;

}
void cleanup_dead_npcs(const std::vector<Actor*>& corpses) {
  for(const auto& c : corpses){
    for(const auto & s : c->bmp){
      surface_list.emplace_back(s.surface);
      texture_list.emplace_back(s.texture);
    }
  }
  world->erase_actors(corpses);
}

void world_tick() {
}

void world_program_exit(){
  world = nullptr;
}
#undef m_debug
