#include <forward_list>
#include "actor.hpp"
#include "player.hpp"
#include "world.hpp"
#include "map.hpp"
#include "wall.hpp"
#include "direction.hpp"

#include "npc-spetsnaz.hpp"
#include "tiled/parser.hpp"

static int hall_width = 80;
static int hall_height = 80;
static SDL_Rect top = {0,0,win_width(),hall_height};
bool top_intersects_with(Player& p) {
	return SDL_HasIntersection(&top,&p.self.rect);
}
void init_world() {
	int status = import_tiled_world("../assets/apartment.csv");
	std::cout << "import_tiled_world status: " << status << "\n";
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
			wall::draw_wall_at(col * CELL_HEIGHT, row * CELL_WIDTH, CELL_WIDTH,CELL_HEIGHT,static_cast<wall::Texture>(val));
			++col;
		}
		col = 0;
	}

	return 0;

}
