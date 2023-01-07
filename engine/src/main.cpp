#include <cstdlib>
#include "world.hpp"
#include "background.hpp"
#include "player.hpp"
#include "movement.hpp"
#include "triangle.hpp"
#include "bullet-pool.hpp"
#include "npc-spetsnaz.hpp"

static int32_t START_X = 0;
static int32_t START_Y = 0;
static int WIN_WIDTH = 512;
static int WIN_HEIGHT = 512;
void ren_clear(){
  SDL_RenderClear(ren);
}
void ren_present(){
		SDL_RenderPresent(ren);
}
int tile_width() { return WIN_WIDTH / 32; }
int win_width() { return WIN_WIDTH; }
int win_height() { return WIN_HEIGHT; }
SDL_Renderer* ren;
	std::unique_ptr<Player> guy = nullptr;
  //std::vector<std::shared_ptr<BulletPool>> bullet_pool;
	std::unique_ptr<World> world = nullptr;
	std::unique_ptr<MovementManager> movement_manager = nullptr;
int main()
{
	static constexpr const char* title = "Xorlock v0.2.0";

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		//std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
		return EXIT_FAILURE;
	}
	START_X = WIN_WIDTH / 2;
	START_Y = WIN_HEIGHT / 2;

	SDL_Window* win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		//std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
		return EXIT_FAILURE;
	}

	ren
		= SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
		//std::cerr << "SDL_CreateRenderer Error" << SDL_GetError() << "\n";
		if (win != nullptr) {
			SDL_DestroyWindow(win);
		}
		SDL_Quit();
		return EXIT_FAILURE;
	}

	guy = std::make_unique<Player>(START_X,START_Y,"../assets/guy-0.bmp");
	world = std::make_unique<World>();
	movement_manager = std::make_unique<MovementManager>();
  init_world();
  npc::init_spetsnaz();

	SDL_bool done = SDL_FALSE;
	int amount = 10;
	int numkeys = 26;
	static constexpr uint8_t KEY_W = 26;
	static constexpr uint8_t KEY_A = 4;
	static constexpr uint8_t KEY_S = 22;
	static constexpr uint8_t KEY_D = 7;
	guy->movement_amount = amount;

	int mouse_x,mouse_y;
	SDL_Event event;
  bg::init();
  static_guy::init();
  plr::set_guy(guy.get());
  bg::draw();
	while (!done) {
		const Uint8* keys = SDL_GetKeyboardState(&numkeys);
		if(keys[KEY_W]){
			movement_manager->wants_to_move(*world,*guy,NORTH);
		}
		if(keys[KEY_A]){
			movement_manager->wants_to_move(*world,*guy,WEST);
		}
		if(keys[KEY_S]){
			movement_manager->wants_to_move(*world,*guy,SOUTH);
		}
		if(keys[KEY_D]){
			movement_manager->wants_to_move(*world,*guy,EAST);
		}

    ren_clear();
#ifdef DRAW_GRID
    draw_grid();
#endif
    draw_world();
		if (SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT){
				done = SDL_TRUE;
				break;
			}
			if(event.type == SDL_MOUSEMOTION) {
				SDL_GetMouseState(&mouse_x,&mouse_y);
	      rotate_guy(*guy,mouse_x,mouse_y);
			}
			if(event.type == SDL_MOUSEBUTTONDOWN) {
			  fire_gun(*guy,mouse_x,mouse_y);
			}
		}
    plr::redraw_guy();
		draw_reticle(*guy,mouse_x,mouse_y);
    npc::spetsnaz_tick();
    fire_tick();
		SDL_RenderPresent(ren);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return EXIT_SUCCESS;
}
