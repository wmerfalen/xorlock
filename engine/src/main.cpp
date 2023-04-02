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

#ifdef REPORT_ERROR
#undef REPORT_ERROR
#endif

#ifdef SHOW_ERRORS
#define REPORT_ERROR(A) std::cerr << "[ERROR][" << __FUNCTION__ << "@" << __FILE__ << ":" << __LINE__ << "]: " << A << "\n";
#else
#define REPORT_ERROR(A)
#endif

static int32_t START_X = 0;
static int32_t START_Y = 0;
static int WIN_WIDTH = 1024;
static int WIN_HEIGHT = 1024;
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
int numkeys = 26;
const Uint8* keys;
static constexpr uint8_t KEY_W = 26;
static constexpr uint8_t KEY_A = 4;
static constexpr uint8_t KEY_S = 22;
static constexpr uint8_t KEY_D = 7;
void handle_movement() {
	keys = SDL_GetKeyboardState(&numkeys);
	if(keys[KEY_W]) {
		movement_manager->wants_to_move(*world,*guy,NORTH);
	}
	if(keys[KEY_A]) {
		movement_manager->wants_to_move(*world,*guy,WEST);
	}
	if(keys[KEY_S]) {
		movement_manager->wants_to_move(*world,*guy,SOUTH);
	}
	if(keys[KEY_D]) {
		movement_manager->wants_to_move(*world,*guy,EAST);
	}
	plr::calc();
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

	guy = std::make_unique<Player>(START_X,START_Y,"../assets/guy-0.bmp");
	world = std::make_unique<World>();
	movement_manager = std::make_unique<MovementManager>();
	init_world();

	int amount = 10;
	guy->movement_amount = amount;

	bg::init();
	plr::set_guy(guy.get());
	bg::draw();
	npc::init_spetsnaz();
	cursor::init();
	viewport::init();
	setup_event_filter();
	bullet::init();
	rng::init();
	timeline::init();
	draw_state::init();
	map::init();
	rmapgen::init();
	while(!done) {
		ren_clear();
		handle_mouse();
		handle_movement();
		draw_world();
		timeline::tick();
		plr::redraw_guy();
		plr::draw_reticle();
		npc::spetsnaz_tick();
		map::tick();
		SDL_RenderPresent(ren);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;
}
