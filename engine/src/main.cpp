#define DEVELOPMENT_MENU 1
#include <cstdlib>
#include "defines.hpp"
#include "world.hpp"
#include "background.hpp"
#include "player.hpp"
#include "draw-state/init.hpp"
#include "draw-state/ammo.hpp"
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
#include "reload.hpp"
#include "weapons/weapon-loader.hpp"
#include "font.hpp"
#include "db.hpp"
#include "sound/gunshot.hpp"
#include "sound/reload.hpp"

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
uint64_t render_time;
void ren_clear() {
	SDL_RenderClear(ren);
}
void ren_present() {
	SDL_RenderPresent(ren);
}
std::unique_ptr<reload::ReloadManager> reload_manager;
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
int numkeys = 255;
const Uint8* keys;
static constexpr uint8_t KEY_W = 26;
static constexpr uint8_t KEY_A = 4;
static constexpr uint8_t KEY_S = 22;
static constexpr uint8_t KEY_D = 7;
static constexpr uint8_t KEY_R = 21;
static constexpr uint8_t KEY_NUM_1 = SDL_SCANCODE_1;
static constexpr uint8_t KEY_NUM_2 = SDL_SCANCODE_2;
static constexpr uint8_t KEY_NUM_3 = SDL_SCANCODE_3;
static constexpr uint8_t KEY_NUM_4 = SDL_SCANCODE_4;
static constexpr uint8_t KEY_NUM_5 = SDL_SCANCODE_5;
static constexpr uint8_t KEY_NUM_6 = SDL_SCANCODE_6;
static constexpr uint8_t KEY_NUM_7 = SDL_SCANCODE_7;
static constexpr uint8_t KEY_NUM_8 = SDL_SCANCODE_8;
static constexpr uint8_t KEY_NUM_9 = SDL_SCANCODE_9;
static constexpr uint8_t KEY_NUM_0 = SDL_SCANCODE_0;
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
	bool reload_key_pressed = keys[KEY_R];
	SDL_Keymod mod = SDL_GetModState();
	if(mod == KMOD_LSHIFT) {
		plr::run(true);
	} else {
		plr::run(false);
	}

	if(gameplay::needs_numeric()) {
		bool num_1 = keys[KEY_NUM_1];
		bool num_2 = keys[KEY_NUM_2];
		bool num_3 = keys[KEY_NUM_3];
		bool num_4 = keys[KEY_NUM_4];
		bool num_5 = keys[KEY_NUM_5];
		bool num_6 = keys[KEY_NUM_6];
		bool num_7 = keys[KEY_NUM_7];
		bool num_8 = keys[KEY_NUM_8];
		bool num_9 = keys[KEY_NUM_9];
		bool num_0 = keys[KEY_NUM_0];
		if(num_1) {
			gameplay::numeric_pressed(1);
		}
		if(num_2) {
			gameplay::numeric_pressed(2);
		}
		if(num_3) {
			gameplay::numeric_pressed(3);
		}
		if(num_4) {
			gameplay::numeric_pressed(4);
		}
		if(num_5) {
			gameplay::numeric_pressed(5);
		}
		if(num_6) {
			gameplay::numeric_pressed(6);
		}
		if(num_7) {
			gameplay::numeric_pressed(7);
		}
		if(num_8) {
			gameplay::numeric_pressed(8);
		}
		if(num_9) {
			gameplay::numeric_pressed(9);
		}
		if(num_0) {
			gameplay::numeric_pressed(0);
		}
	}
	if(reload_key_pressed && reload_manager->is_reloading() == false) {
		reload::reload_response_t response = reload_manager->start_reload();
		switch(response) {
			case reload::reload_response_t::NOT_ENOUGH_AMMO:
				std::cout << "not enough ammo\n";
				break;
			case reload::reload_response_t::CURRENTLY_RELOADING:
				std::cout << "currently reloading\n";
				break;
			case reload::reload_response_t::STARTING_RELOAD:
				std::cout << "Starting reload\n";
				break;
			case reload::reload_response_t::CLIP_FULL:
				std::cout << "clip full\n";
				break;
			default:
				std::cout << "default\n";
				break;
		}
	}
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
				if(reload_manager->is_reloading() == false) {
					plr::start_gun();
				}
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
int main(int argc, char** argv) {
	static constexpr const char* title = "Xorlock v0.2.0";

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		REPORT_ERROR("SDL_Init Error: " << SDL_GetError());
		return EXIT_FAILURE;
	}
  if(SDL_Init(SDL_INIT_AUDIO) != 0){
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
	std::cout << "START_X: " << START_X << " " << "START_Y: " << START_Y << "\n";

	guy = std::make_unique<Player>(START_X,START_Y,"../assets/guy-0.bmp", BASE_MOVEMENT_AMOUNT);
	world = std::make_unique<World>();
	movement_manager = std::make_unique<MovementManager>();
	init_world();
  sound::init();
  sound::reload::init();
	rng::init();
	db::init();
	bg::init();
	plr::set_guy(guy.get());
  wpn::vault::init(argc,argv); // Defined in weapons/weapon-loader.hpp
	guy->equip_weapon(wpn::weapon_t::WPN_MP5);
	bg::draw();
	cursor::init();
	font::init();
	viewport::init();
	setup_event_filter();
	bullet::init();
	timeline::init();
	draw_state::init();
	map::init();
	rmapgen::init();
	gameplay::init();
	npc::init_spetsnaz();
	wall::init();
	movement::init(movement_manager.get());
	draw_state::ammo::init();
	reload_manager = std::make_unique<reload::ReloadManager>(guy->clip_size,*(guy->ammo),*(guy->total_ammo),*(guy->wpn_stats));
	static constexpr uint32_t target_render_time = 25000;
  sound::start_track("track-01-camo");
	while(!done) {
		timeline::start_timer();
		ren_clear();
		handle_mouse();
		handle_movement();
		draw_world();
		map::tick();
		timeline::tick();
		if(reload_manager->is_reloading()) {
			plr::update_reload_state(reload_manager->tick());
		}
		plr::redraw_guy();
		npc::spetsnaz_tick();
		plr::draw_reticle();
		draw::blatant();
		draw::overlay_grid();
		gameplay::tick();
		draw::tick_timeline();
		SDL_RenderPresent(ren);
		render_time = timeline::stop_timer();
		if(render_time < target_render_time) {
			::usleep(target_render_time - render_time);
		}
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	font::quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
