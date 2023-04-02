#ifndef __TIMELINE_HEADER__
#define __TIMELINE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "clock.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "graphical-decay.hpp"
#include "extern.hpp"

namespace timeline {

	const std::size_t& next_decay_index();
	void register_timeline_event(
	    int count,
	    interval_t n,
	    timeline::callback_t f);

	void hide_guy_in(int count,interval_t n);
	void init();
	void dispatch_slice(int ms);
	void always_2ms();
	void always_5ms();
	void always_10ms();
	void always_15ms();
	void always_50ms();
	void always_250ms();
	void always_500ms();
	void always_1sec();
	void tick();

};

#endif
