#include <forward_list>
#include "actor.hpp"
#include "player.hpp"
#include "world.hpp"
#include "map.hpp"
#include "colors.hpp"
#include "wall.hpp"
#include "direction.hpp"

#include "npc-spetsnaz.hpp"
#include "tiled/parser.hpp"

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

std::vector<wall::Wall*> get_walkable_toward(const Direction& dir,wall::Wall* from) {
	std::vector<wall::Wall*> walkable;
	std::size_t iteration = 1;
	std::size_t size_of = wall::walls.size();
	std::size_t wall_ctr = 0;
	while(wall_ctr < size_of) {
		wall_ctr = 0;
		for(const auto& w : wall::walls) {
			++wall_ctr;
			if(dir == NORTH) {
				if(w->rect.y == from->rect.y - (CELL_HEIGHT * iteration) && w->rect.x == from->rect.x) {
					if(!w->walkable) {
						return walkable;
					}
					walkable.emplace_back(w.get());
					++iteration;
					break;
				}
			} else if(dir == SOUTH) {
				if(w->rect.y == from->rect.y + (CELL_HEIGHT * iteration) && w->rect.x == from->rect.x) {
					if(!w->walkable) {
						return walkable;
					}
					walkable.emplace_back(w.get());
					++iteration;
					break;
				}
			} else if(dir == EAST) {
				if(w->rect.x == from->rect.x + (CELL_WIDTH * iteration) && w->rect.y == from->rect.y) {
					if(!w->walkable) {
						return walkable;
					}
					walkable.emplace_back(w.get());
					++iteration;
					break;
				}
			} else if(dir == WEST) {
				if(w->rect.x == from->rect.x - (CELL_WIDTH * iteration) && w->rect.y == from->rect.y) {
					if(!w->walkable) {
						return walkable;
					}
					walkable.emplace_back(w.get());
					++iteration;
					break;
				}
			}
		}
	}
	return walkable;
}

std::array<wall::Wall*,8> get_surrounding_walls(wall::Wall* from) {
	std::array<wall::Wall*,8> neighbors = {nullptr};
	std::size_t i=0;
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
		if((from->rect.x - from->rect.w == w->rect.x) &&
		        (from->rect.y - from->rect.h == w->rect.y)) {
			/**
			 * Found NW
			 */
			++i;
			neighbors[nb::NW] = w.get();
		} else if((from->rect.y - from->rect.h == w->rect.y) &&
		          (from->rect.x == w->rect.x)) {
			/**
			 * Found N
			 */
			++i;
			neighbors[nb::N] = w.get();
		} else if((from->rect.x + from->rect.w == w->rect.x) &&
		          (from->rect.y - from->rect.h == w->rect.y)) {
			/**
			 * Found NE
			 */
			++i;
			neighbors[nb::NE] = w.get();
		} else if((from->rect.x - from->rect.w == w->rect.x) &&
		          (from->rect.y == w->rect.y)) {
			/**
			 * Found W
			 */

			++i;
			neighbors[nb::W] = w.get();
		} else if((from->rect.x - from->rect.w == w->rect.x) &&
		          (from->rect.y + from->rect.h == w->rect.y)) {
			/**
			 * Found SW
			*/
			++i;
			neighbors[nb::SW] = w.get();
		} else if((from->rect.x == w->rect.x) &&
		          (from->rect.y + from->rect.h == w->rect.y)) {
			/**
			 * Found S
			*/
			++i;
			neighbors[nb::S] = w.get();
		} else if((from->rect.x + from->rect.w == w->rect.x) &&
		          (from->rect.y + from->rect.h == w->rect.y)) {
			/**
			 * Found SE
			*/
			++i;
			neighbors[nb::SE] = w.get();
		} else if((from->rect.x + from->rect.w == w->rect.x) &&
		          (from->rect.y == w->rect.y)) {
			/**
			 * Found E
			*/
			++i;
			neighbors[nb::E] = w.get();
		}


		if(i == 8) {
			break;
		}
	}
	return neighbors;
}

bool walkable(wall::Wall* w) {
	return w && w->any_of(wall::WALKABLE);
}
using txt_t = wall::Texture;
bool wall_is(wall::Wall* w,const txt_t& t) {
	return w && w->type == t;
}
bool walkable(std::array<wall::Wall*,8>* nbrs,std::vector<nb>&& dirs) {
	for(auto&& d : dirs) {
		if(!walkable((*nbrs)[d])) {
			return false;
		}
	}
	return true;
}
void find_edge_connections() {
	std::vector<wall::Wall*> found;
	for(const auto& w : wall::walls) {
		if(w->is_gateway == false) {
			continue;
		}
		for(const auto dir : {
		            NORTH,SOUTH,EAST,WEST
		        }) {
			found = get_walkable_toward(dir,w.get());
			for(auto& tile : found) {
				tile->draw_color = colors::red();
				++tile->connections;
			}
		}
	}
}
void find_edges() {
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
		if(wall_is(w.get(),txt_t::BR_TR_CRNR) && wall_is(nbrs[nb::S],txt_t::BR_BR_CRNR) &&
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
		if(wall_is(w.get(),txt_t::BR_TL_CRNR) && wall_is(nbrs[nb::S],txt_t::BR_BL_CRNR) &&
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
		if(wall_is(w.get(),txt_t::BLD_BL_CRNR) &&
		        wall_is(nbrs[nb::N],txt_t::BLD_LWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_TWALL) &&
		        wall_is(nbrs[nb::W],txt_t::BLD_TWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_TWALL) &&
		        wall_is(nbrs[nb::E],txt_t::BLD_TWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_TR_CRNR) &&
		        wall_is(nbrs[nb::W],txt_t::BLD_TWALL) &&
		        wall_is(nbrs[nb::S],txt_t::BLD_RWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_LWALL) &&
		        wall_is(nbrs[nb::S],txt_t::BLD_LWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_BWALL) &&
		        wall_is(nbrs[nb::W],txt_t::BLD_BWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_BWALL) &&
		        wall_is(nbrs[nb::E],txt_t::BLD_BWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_BR_CRNR) &&
		        wall_is(nbrs[nb::N],txt_t::BLD_RWALL) &&
		        wall_is(nbrs[nb::W],txt_t::BLD_BWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_RWALL) &&
		        wall_is(nbrs[nb::N],txt_t::BLD_RWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_RWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_RWALL) &&
		        wall_is(nbrs[nb::S],txt_t::BLD_RWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_RWALL) &&
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
		if(wall_is(w.get(),txt_t::BLD_TL_CRNR) &&
		        wall_is(nbrs[nb::E],txt_t::BLD_TWALL) &&
		        wall_is(nbrs[nb::S],txt_t::BLD_LWALL) &&
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
void init_world() {
	int status = import_tiled_world("../assets/apartment.csv");
	std::cout << "import_tiled_world status: " << status << "\n";
	find_edges();
	find_edge_connections();
	auto gw_indexed = index_gateways();
	std::cout << gw_indexed << " gateways indexed\n";
}

void draw_world() {
}

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
	fclose(fp);
	for(std::size_t row = 0; row < map.size(); row++) {
		std::size_t col = 0;
		for(const auto& val : map[row]) {
			wall::draw_wall_at(START_X + (col * CELL_HEIGHT), START_Y + (row * CELL_WIDTH), CELL_WIDTH,CELL_HEIGHT,static_cast<wall::Texture>(val));
			++col;
		}
		col = 0;
	}

	return 0;

}

void world_tick() {
}
