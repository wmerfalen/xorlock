#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>

int main()
{

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
    return EXIT_FAILURE;
  }

  SDL_Window* win = SDL_CreateWindow("Xorlock v0.0.1", 0, 0, 1024, 1024, SDL_WINDOW_SHOWN);
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

  SDL_Surface* bmp = SDL_LoadBMP("../assets/guy.bmp");
  if (bmp == nullptr) {
    std::cerr << "SDL_LoadBMP Error: " << SDL_GetError() << "\n";
    if (ren != nullptr) {
      SDL_DestroyRenderer(ren);
    }
    if (win != nullptr) {
      SDL_DestroyWindow(win);
    }
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, bmp);
  if (tex == nullptr) {
    std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << "\n";
    if (bmp != nullptr) {
      SDL_FreeSurface(bmp);
    }
    if (ren != nullptr) {
      SDL_DestroyRenderer(ren);
    }
    if (win != nullptr) {
      SDL_DestroyWindow(win);
    }
    SDL_Quit();
    return EXIT_FAILURE;
  }
  SDL_FreeSurface(bmp);

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
  while (!done) {
    SDL_Event event;
    const Uint8* keys = SDL_GetKeyboardState(&numkeys);
    SDL_RenderClear(ren);
    if(keys[KEY_W]){
      Destination.y -= amount;
    }
    if(keys[KEY_A]){
      Destination.x -= amount;
    }
    if(keys[KEY_S]){
      Destination.y += amount;
    }
    if(keys[KEY_D]){
      Destination.x += amount;
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
