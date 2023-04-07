#ifndef __CURSOR_HEADER__
#define __CURSOR_HEADER__
#include <SDL2/SDL.h>
#include <iostream>



namespace cursor {
	void init();
	void use_reticle();
	void disable_cursor();
	void update_mouse();
	int& mx();
	int& my();
};


#endif
