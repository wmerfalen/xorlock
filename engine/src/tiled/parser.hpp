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
		Parser(const std::string& _file_name);
		Parser(const Parser&) = delete;
		~Parser() = default;

	};
	int parser_main();
};


#endif
