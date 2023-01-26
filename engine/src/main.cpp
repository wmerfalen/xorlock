#include <cstdlib>
#include "defines.hpp"
#include "world.hpp"
#include "background.hpp"
#include "player.hpp"
#include "movement.hpp"
#include "triangle.hpp"
#include "bullet-pool.hpp"
#include "npc-spetsnaz.hpp"
#include "cursor.hpp"
#include "tick.hpp"
#include "viewport.hpp"
#include "clock.hpp"

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
static int WIN_WIDTH = 512;
static int WIN_HEIGHT = 512;
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
	static constexpr int NUM_PEEK = 4;
	//int count = SDL_PeepEvents(&event, //SDL_Event * events,
	//                           NUM_PEEK,      //int numevents,
	//                           SDL_PEEKEVENT, //SDL_eventaction action,
	//                           SDL_FIRSTEVENT, // Uint32 minType,
	//                           SDL_LASTEVENT // Uint32 maxType);
	//                          );
	//if(count <= 0) {
	//	return true;
	//}
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_MOUSEBUTTONDOWN) {
			//plr::start_gun(mouse_x,mouse_y);
			plr::fire_weapon();
		}
		if(event.type == SDL_MOUSEMOTION) {
			SDL_GetMouseState(&mouse_x,&mouse_y);
			cursor::update_mouse(mouse_x,mouse_y);
			plr::rotate_guy();
		}
		//if(event.type == SDL_MOUSEBUTTONUP) {
		//		plr::stop_gun();
		//	}
		if(event.type == SDL_QUIT) {
			done = SDL_TRUE;
			return false;
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
	static_guy::init();
	plr::set_guy(guy.get());
	bg::draw();
	npc::init_spetsnaz();
	cursor::init();
	tick::init();
	viewport::init();
	clk::init();
	while(!done) {
		tick::inc();
		ren_clear();
		handle_mouse();
		handle_movement();
		handle_mouse();
		plr::redraw_guy();
		bullet::tick();
		//draw_reticle(*guy,mouse_x,mouse_y);
		//npc::spetsnaz_tick();
		//auto loop_miliseconds = clk::end();
		//int time_to_wait = 10 - loop_miliseconds.count();
		//if(time_to_wait > 0) {
		SDL_Delay(20);
		//}
		SDL_RenderPresent(ren);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;
}
