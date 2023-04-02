#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string_view>
#include <memory>
#include "hallway.hpp"
#include "world.hpp"
#include "triangle.hpp"
SDL_Texture* ContinuousLTRWall::initial_texture() {
	return self.bmp[0].texture;
}

SDL_Texture* Hallway::initial_texture() {
	return self.bmp[0].texture;
}
