#ifndef __VIEWPORT_HEADER__
#define __VIEWPORT_HEADER__
#include <SDL2/SDL.h>
#include <iostream>



namespace viewport {
	void init();
	int min_x();
	int max_x();
	int min_y();
	int max_y();

	void set_min_x(int i);
	void set_max_x(int i);
	void set_min_y(int i);
	void set_max_y(int i);
	std::string report();
};


#endif
