#ifndef __CLOCK_HEADER__
#define __CLOCK_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace clk {
	using unit_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
	static unit_t m_start;
	static unit_t m_end;
	using duration_t = std::chrono::duration<double>;
	static constexpr std::size_t FRAMES_PER_SECOND = 60;

	void init() {
	}
	auto now() {
		return std::chrono::high_resolution_clock::now();
	}

	auto start() {
		return m_start = now();
	}
	duration_t end() {
		return (now() - m_start) * 1000;
	}
	void delay_for_frame() {
		int ms = 1000 / FRAMES_PER_SECOND - end().count();
		if(ms > 0) {
			SDL_Delay(ms);
		}
	}

};

#endif
