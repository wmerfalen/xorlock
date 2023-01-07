#include <cstdlib>
#include "world.hpp"
#include "player.hpp"

static std::unique_ptr<Player> guy = nullptr;
static int32_t START_X = 0;
static int32_t START_Y = 0;
static int WIN_WIDTH = 1024;
static int WIN_HEIGHT = 1024;
int main()
{
  static constexpr const char* title = "Xorlock v0.0.1";

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
    return EXIT_FAILURE;
  }
  START_X = WIN_WIDTH / 2;
  START_Y = WIN_HEIGHT / 2;

  SDL_Window* win = SDL_CreateWindow(title, 0, 0, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
  if (win == nullptr) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
    return EXIT_FAILURE;
  }

  SDL_Renderer* ren
    = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == nullptr) {
    std::cerr << "SDL_CreateRenderer Error" << SDL_GetError() << "\n";
    if (win != nullptr) {
      SDL_DestroyWindow(win);
    }
    SDL_Quit();
    return EXIT_FAILURE;
  }
  guy = std::make_unique<Player>(START_X,START_Y,"../assets/guy.bmp");

  SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, guy->self.bmp);
  if (tex == nullptr) {
    std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << "\n";
    if (ren != nullptr) {
      SDL_DestroyRenderer(ren);
    }
    if (win != nullptr) {
      SDL_DestroyWindow(win);
    }
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_bool done = SDL_FALSE;
  //SDL_StartTextInput();
  int X,Y,Width,Height;
  X = 0;
  Y = 0;
  Width = 80;
  Height = 80;
  SDL_Rect Destination = {X, Y, Width, Height};
  int amount = 10;
  int numkeys = 26;
  static constexpr uint8_t KEY_W = 26;
  static constexpr uint8_t KEY_A = 4;
  static constexpr uint8_t KEY_S = 22;
  static constexpr uint8_t KEY_D = 7;
  bool clear = false;

  while (!done) {
    SDL_Event event;
    const Uint8* keys = SDL_GetKeyboardState(&numkeys);
    if(keys[KEY_W]){
      clear = true;
      Destination.y -= amount;
      guy->wants_to_move(NORTH);
    }
    if(keys[KEY_A]){
      clear = true;
      Destination.x -= amount;
      guy->wants_to_move(WEST);
    }
    if(keys[KEY_S]){
      clear = true;
      Destination.y += amount;
      guy->wants_to_move(SOUTH);
    }
    if(keys[KEY_D]){
      clear = true;
      Destination.x += amount;
      guy->wants_to_move(EAST);
    }
    if(clear){
      SDL_RenderClear(ren);
      clear = false;
    }

    if (SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT){
        done = SDL_TRUE;
        break;
      }
    }
    SDL_RenderCopy(ren, tex, nullptr, &Destination);
    SDL_RenderPresent(ren);
  }

  SDL_DestroyTexture(tex);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return EXIT_SUCCESS;
}
