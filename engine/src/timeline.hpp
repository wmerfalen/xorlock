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
	using unit_t =	std::chrono::time_point< std::chrono::system_clock >;
	static unit_t m_start;
	static unit_t m_end;
	using duration_t = std::chrono::duration<double>;
	static constexpr std::size_t FRAMES_PER_SECOND = 10;
	static constexpr std::size_t MILLISECONDS_PER_FRAME = 1000 / FRAMES_PER_SECOND;
	static unit_t m_2ms_start;
	static unit_t m_2ms_end;
	static unit_t m_5ms_start;
	static unit_t m_5ms_end;
	static unit_t m_10ms_start;
	static unit_t m_10ms_end;
	static unit_t m_15ms_start;
	static unit_t m_15ms_end;
	static unit_t m_50ms_start;
	static unit_t m_50ms_end;
	static unit_t m_100ms_start;
	static unit_t m_100ms_end;
	static unit_t m_250ms_start;
	static unit_t m_250ms_end;
	static unit_t m_500ms_start;
	static unit_t m_500ms_end;
	static unit_t m_1sec_start;
	static unit_t m_1sec_end;
	static auto diff = std::chrono::duration_cast< std::chrono::milliseconds >(m_end - m_start);
	static auto diff_2ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_2ms_end- m_2ms_start);
	static auto diff_5ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_5ms_end- m_5ms_start);
	static auto diff_10ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_10ms_end- m_10ms_start);
	static auto diff_15ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_15ms_end- m_15ms_start);
	static auto diff_50ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_50ms_end- m_50ms_start);
	static auto diff_250ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_250ms_end- m_250ms_start);
	static auto diff_500ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_500ms_end- m_500ms_start);
	static auto diff_1sec = std::chrono::duration_cast< std::chrono::milliseconds >(m_1sec_end- m_1sec_start);

	static constexpr std::size_t GR_DECAY_SIZE = 1024;
	static std::array<grdecay::asset,GR_DECAY_SIZE> m_decay_list;
	static std::size_t m_decay_index;
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
