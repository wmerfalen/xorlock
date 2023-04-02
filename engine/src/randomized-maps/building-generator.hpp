#ifndef __RMAPGEN_BUILDING_GENERATOR_HEADER__
#define __RMAPGEN_BUILDING_GENERATOR_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "../actor.hpp"
#include "../map.hpp"
#include "../world.hpp"
#include "../triangle.hpp"
#include "../coordinates.hpp"
#include <map>
#include "../extern.hpp"
#include "../draw.hpp"

namespace rmapgen {
	struct BuildingGenerator {
		BuildingGenerator() :
			ready(true),
			generated(false) {
		}
		bool ready;
		bool generated;

		/** Copy constructor */
		BuildingGenerator(const BuildingGenerator& other) = delete;

	};

	void init() {
		std::cout << "rmapgen::init()\n";
	}

}; //end namespace rmapgen

#endif
