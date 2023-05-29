#define DEVELOPMENT_MENU 1
#include <cstdlib>
#include "defines.hpp"
#include "world.hpp"
#include "background.hpp"
#include "player.hpp"
#include "draw-state/init.hpp"
#include "movement.hpp"
#include "triangle.hpp"
#include "bullet-pool.hpp"
#include "npc-spetsnaz.hpp"
#include "cursor.hpp"
#include "tick.hpp"
#include "viewport.hpp"
#include "timeline.hpp"
#include "map.hpp"
#include "randomized-maps/building-generator.hpp"
#include "gameplay.hpp"
#include "font.hpp"

#ifdef REPORT_ERROR
#undef REPORT_ERROR
#endif

#ifdef SHOW_ERRORS
#define REPORT_ERROR(A) std::cerr << "[ERROR][" << __FUNCTION__ << "@" << __FILE__ << ":" << __LINE__ << "]: " << A << "\n";
#else
#define REPORT_ERROR(A)
#endif

static int WIN_WIDTH = 1024;
static int WIN_HEIGHT = 1024;
int32_t START_X = WIN_WIDTH / 2;
int32_t START_Y = WIN_HEIGHT / 2;
void ren_clear() {
	SDL_RenderClear(ren);
}
void ren_present() {
	SDL_RenderPresent(ren);
}
int tile_width() {
	return WIN_WIDTH / 32;
}
int win_width() {
	return WIN_WIDTH;
}
int win_height() {
	return WIN_HEIGHT;
}
int mouse_x,mouse_y;
SDL_Event event;
SDL_Renderer* ren;
SDL_bool done = SDL_FALSE;
std::unique_ptr<Player> guy = nullptr;
std::unique_ptr<World> world = nullptr;
std::unique_ptr<MovementManager> movement_manager = nullptr;
void setup_event_filter() {
	std::vector<uint32_t> disable = {
		SDL_APP_TERMINATING,
		SDL_APP_LOWMEMORY,
		SDL_APP_WILLENTERBACKGROUND,
		SDL_APP_DIDENTERBACKGROUND,
		SDL_APP_WILLENTERFOREGROUND,
		SDL_APP_DIDENTERFOREGROUND,
		SDL_LOCALECHANGED,
		SDL_DISPLAYEVENT,
		SDL_WINDOWEVENT,
		SDL_SYSWMEVENT,
		SDL_TEXTEDITING,
		SDL_TEXTINPUT,
		SDL_KEYMAPCHANGED,
		SDL_MOUSEWHEEL,
		SDL_JOYAXISMOTION,
		SDL_JOYBALLMOTION,
		SDL_JOYHATMOTION,
		SDL_JOYBUTTONDOWN,
		SDL_JOYBUTTONUP,
		SDL_JOYDEVICEADDED,
		SDL_JOYDEVICEREMOVED,
		SDL_CONTROLLERAXISMOTION,
		SDL_CONTROLLERBUTTONDOWN,
		SDL_CONTROLLERBUTTONUP,
		SDL_CONTROLLERDEVICEADDED,
		SDL_CONTROLLERDEVICEREMOVED,
		SDL_CONTROLLERDEVICEREMAPPED,
		SDL_FINGERDOWN,
		SDL_FINGERUP,
		SDL_FINGERMOTION,
		SDL_DOLLARGESTURE,
		SDL_DOLLARRECORD,
		SDL_MULTIGESTURE,
		SDL_CLIPBOARDUPDATE,
		SDL_DROPFILE,
		SDL_DROPTEXT,
		SDL_DROPBEGIN,
		SDL_DROPCOMPLETE,
		SDL_AUDIODEVICEADDED,
		SDL_AUDIODEVICEREMOVED,
		SDL_RENDER_TARGETS_RESET,
		SDL_RENDER_DEVICE_RESET,
	};
	std::vector<uint32_t> enable = {
		SDL_KEYDOWN,
		SDL_KEYUP,
		SDL_MOUSEMOTION,
		SDL_MOUSEBUTTONDOWN,
		SDL_MOUSEBUTTONUP,
		//SDL_USEREVENT,
		//SDL_LASTEVENT,
	};
	for(const auto& d : disable) {
		SDL_EventState(d,SDL_DISABLE);
	}
	for(const auto& e : enable) {
		SDL_EventState(e,SDL_ENABLE);
	}

}
bool dev_menu() {
	auto r = plr::get_effective_rect();
	std::cout << dbg::dump(r) << "\n";
	std::cout << plr::self()->world_x << "x" << plr::self()->world_y << "\n";
	return true;
}
int numkeys = 44;
const Uint8* keys;
static constexpr uint8_t KEY_W = 26;
static constexpr uint8_t KEY_A = 4;
static constexpr uint8_t KEY_S = 22;
static constexpr uint8_t KEY_D = 7;
static constexpr uint8_t SPACE_BAR = 44;
void handle_movement() {
	keys = SDL_GetKeyboardState(&numkeys);
#ifdef DEVELOPMENT_MENU
	if(keys[SPACE_BAR]) {
		if(!dev_menu()) {
			return;
		}
	}
#endif
	bool north_east = keys[KEY_W] && keys[KEY_D];
	bool north_west = keys[KEY_W] && keys[KEY_A];
	bool south_east = keys[KEY_S] && keys[KEY_D];
	bool south_west = keys[KEY_S] && keys[KEY_A];
	bool north = keys[KEY_W] && (!keys[KEY_D] && !keys[KEY_A]);
	bool south = keys[KEY_S] && (!keys[KEY_D] && !keys[KEY_A]);
	bool east = keys[KEY_D] && (!keys[KEY_W] && !keys[KEY_S]);
	bool west = keys[KEY_A] && (!keys[KEY_W] && !keys[KEY_S]);
	if(north_east) {
		movement_manager->wants_to_move(*world,NORTH_EAST);
	} else if(north_west) {
		movement_manager->wants_to_move(*world,NORTH_WEST);
	} else if(south_east) {
		movement_manager->wants_to_move(*world,SOUTH_EAST);
	} else if(south_west) {
		movement_manager->wants_to_move(*world,SOUTH_WEST);
	} else if(north) {
		movement_manager->wants_to_move(*world,NORTH);
	} else if(south) {
		movement_manager->wants_to_move(*world,SOUTH);
	} else if(east) {
		movement_manager->wants_to_move(*world,EAST);
	} else if(west) {
		movement_manager->wants_to_move(*world,WEST);
	}

	guy->calc();
}
bool handle_mouse() {
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
				plr::start_gun();
				break;
			case SDL_MOUSEBUTTONUP:
				plr::stop_gun();
				break;
			case SDL_MOUSEMOTION:
				cursor::update_mouse();
				plr::rotate_guy();
				break;
			case SDL_QUIT:
				done = SDL_TRUE;
				return false;
			default:
				break;
		}
	}
	return true;
}
int main() {
	static constexpr const char* title = "Xorlock v0.2.0";

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		REPORT_ERROR("SDL_Init Error: " << SDL_GetError());
		return EXIT_FAILURE;
	}
	START_X = WIN_WIDTH / 2;
	START_Y = WIN_HEIGHT / 2;

	SDL_Window* win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
	if(win == nullptr) {
		REPORT_ERROR("SDL_CreateWindow Error: " << SDL_GetError());
		return EXIT_FAILURE;
	}

	ren
	    = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(ren == nullptr) {
		REPORT_ERROR("SDL_CreateRenderer Error" << SDL_GetError());
		if(win != nullptr) {
			SDL_DestroyWindow(win);
		}
		SDL_Quit();
		return EXIT_FAILURE;
	}
#ifdef USE_PATH_TESTING_NORTH_EAST
	START_X = 1024 / 2 + 500;
	START_Y = 1024 / 2 - 500;
#endif
	std::cout << "START_X: " << START_X << " " << "START_Y: " << START_Y << "\n";

	guy = std::make_unique<Player>(START_X,START_Y,"../assets/guy-0.bmp", BASE_MOVEMENT_AMOUNT);
	world = std::make_unique<World>();
	movement_manager = std::make_unique<MovementManager>();
	init_world();

	bg::init();
	plr::set_guy(guy.get());
	bg::draw();
	cursor::init();
	font::init();
	viewport::init();
	setup_event_filter();
	bullet::init();
	rng::init();
	timeline::init();
	draw_state::init();
	for(int i=0; i < 100; i++) {
		movement_manager->wants_to_move(*world,SOUTH);
	}
	for(int i=0; i < 70; i++) {
		movement_manager->wants_to_move(*world,EAST);
	}
	for(int i=0; i < 70; i++) {
		movement_manager->wants_to_move(*world,SOUTH);
	}
	map::init();
	rmapgen::init();
	gameplay::init();
	npc::init_spetsnaz();
	while(!done) {
		gameplay::tick();
		ren_clear();
		handle_mouse();
		handle_movement();
		draw_world();
		map::tick();
		timeline::tick();
		plr::redraw_guy();
		npc::spetsnaz_tick();
		plr::draw_reticle();
		bullet::tick();
		draw::blatant();
		draw::overlay_grid();
		SDL_RenderPresent(ren);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	font::quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
