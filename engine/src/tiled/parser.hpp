#ifndef __TILED_PARSER_HEADER__
#define __TILED_PARSER_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <string_view>
#include <string>
#include "../actor.hpp"
#include "../world.hpp"

namespace tiled {
	struct Parser {
		Parser() = delete;
		Parser(std::string_view _file_name, std::size_t _map_width, std::size_t _map_height);
		Parser(const Parser&) = delete;
		~Parser() = default;

	};
};


#endif
