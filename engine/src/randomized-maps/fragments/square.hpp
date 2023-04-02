#ifndef __RMAPGEN_FRAGMENTS_SQUARE_HEADER__
#define __RMAPGEN_FRAGMENTS_SQUARE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include <map>
#include "../../actor.hpp"
#include "../../world.hpp"
#include "../../triangle.hpp"
#include "../../coordinates.hpp"
#include "../../extern.hpp"
#include "../../map.hpp"

namespace rmapgen::fragments {
	struct Square {
		Square() :
			ready(false) {
		}
		bool ready;

		/** Copy constructor */
		Square(const Square& other) = delete;

	};

}; //end namespace rmapgen

#endif
