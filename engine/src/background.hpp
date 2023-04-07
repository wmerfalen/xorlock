#ifndef __BACKGROUND_HEADER__
#define __BACKGROUND_HEADER__
#include <SDL2/SDL.h>

#include <memory>
#include <array>
#include "window.hpp"

using Color = std::array<uint8_t,3>;
static constexpr Color DEFAULT_BG = {0,0,0};

struct Background {
	Color color;
	uint64_t width;
	uint64_t height;
	Background() = delete;
	Background(const Background&) = delete;
	Background(const Color& a) : color(a) {
		width = win_width();
		height = win_height();
	}
	void draw();

	~Background() = default;

};

namespace bg {
	static std::unique_ptr<Background> background;

	void init();

	void draw();
};

#endif
