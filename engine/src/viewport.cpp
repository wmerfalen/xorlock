#include <SDL2/SDL.h>
#include <iostream>

#include "viewport.hpp"
#include "window.hpp"


namespace viewport {
	static int _min_x;
	static int _max_x;
	static int _min_y;
	static int _max_y;
	int min_x() {
		return _min_x;
	}
	int max_x() {
		return _max_x;
	}
	int min_y() {
		return _min_y;
	}
	int max_y() {
		return _max_y;
	}
	void init() {
		_min_x = 0;
		_max_x = win_width();
		_min_y = 0;
		_max_y = win_height();
	}
	std::string report() {
		std::string s = "viewport: ";
		s += "min_x: ";
		s += std::to_string(_min_x);
		s += "max_x: ";
		s += std::to_string(_max_x);
		s += "min_y: ";
		s += std::to_string(_min_y);
		s += "max_y: ";
		s += std::to_string(_max_y);
		std::cout << s << "\n";
		return s;
	}
	void set_min_x(int i) {
		_min_x = i;
	}
	void set_max_x(int i) {
		_max_x = i;
	}
	void set_min_y(int i) {
		_min_y = i;
	}
	void set_max_y(int i) {
		_max_y = i;
	}
	void draw() {
		SDL_Rect r;
		r.x = _min_x;
		r.y = _min_y;
		r.w = _max_x;
		r.h = _max_y;
		SDL_RenderDrawRect(ren,&r);
	}
};
