#ifndef __TIMELINE_HEADER__
#define __TIMELINE_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <functional>
//#include "clock.hpp"
//#include "bullet.hpp"
//#include "player.hpp"

namespace timeline {
	using callback_t = std::function<void(void*)>;
	enum interval_t : uint16_t {
		MS_2 = 2,
		MS_5 = 5,
		MS_10 = 10,
		MS_15 = 15,
		MS_20 = 20,
		MS_25 = 25,
		MS_50 = 50,
		MS_100 = 100,
		MS_250 = 250,
		MS_500 = 500,
		SEC_1 = 1000,
	};
	using seconds_t = uint32_t;
	extern void register_timeline_event(
	    int count,
	    interval_t n,
	    timeline::callback_t f);
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
