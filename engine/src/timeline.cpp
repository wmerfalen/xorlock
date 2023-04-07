#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "timeline.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "graphical-decay.hpp"


namespace timeline {
	static constexpr std::size_t GR_DECAY_SIZE = 1024;
	static std::array<grdecay::asset,GR_DECAY_SIZE> m_decay_list;
	static std::size_t m_decay_index;
	using unit_t =	std::chrono::time_point< std::chrono::system_clock >;
	static unit_t m_start;
	static unit_t m_end;
	using duration_t = std::chrono::duration<double>;
	//static constexpr std::size_t FRAMES_PER_SECOND = 10;
	//static constexpr std::size_t MILLISECONDS_PER_FRAME = 1000 / FRAMES_PER_SECOND;
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
	//static unit_t m_100ms_start;
	//static unit_t m_100ms_end;
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
	const std::size_t& next_decay_index() {
		if(m_decay_index + 1 >= GR_DECAY_SIZE - 1) {
			m_decay_index = 0;
			return m_decay_index;
		}
		return ++m_decay_index;
	}
	void register_timeline_event(
	    int count,
	    interval_t n,
	    timeline::callback_t f) {
		auto& ref = m_decay_list[next_decay_index()];
		ref.id = grdecay::asset_id();
		ref.done = false;
		ref.run_me = true;
		ref.when = (int)n;
		ref.ctr = count;
		ref.func = f;
	}

	void hide_guy_in(int count,interval_t n) {
		static timeline::callback_t func = [](void* _in_asset) {
			grdecay::asset* a = reinterpret_cast<grdecay::asset*>(_in_asset);
			if(a->ctr - 1 == 0) {
				draw_state::player::hide_guy();
			}
		};
		register_timeline_event(count,n,func);
	}
	void init() {
		m_2ms_start = m_5ms_start = m_15ms_start = m_1sec_start = m_500ms_start = m_50ms_start = m_250ms_start = m_10ms_start = clk::now();
		m_decay_index = 0;
		for(std::size_t i=0; i < GR_DECAY_SIZE; ++i) {
			m_decay_list[i].run_me = false;
		}
	}
	void dispatch_slice(int ms) {
		for(auto& decay : m_decay_list) {
			if(!decay.run_me) {
				continue;
			}
			if(decay.when == ms) {
				decay.func((void*)&decay);
				decay.ctr -= 1;
			}
			if(decay.ctr <= 0) {
				decay.done = true;
				decay.run_me = false;
			}
		}
	}
	void always_2ms() {
		bullet::tick();
		//if(plr::ms_registration() == MS_2) {
		//	if(plr::should_fire()) {
		//		plr::fire_weapon();
		//	}
		//}
	}
	void always_5ms() {
		bullet::tick();
		//if(plr::ms_registration() == MS_5) {
		//	if(plr::should_fire()) {
		//		plr::fire_weapon();
		//	}
		//}
	}
	void always_10ms() {
		bullet::tick();
		//if(plr::ms_registration() == MS_10) {
		//	if(plr::should_fire()) {
		//		plr::fire_weapon();
		//	}
		//}
	}
	void always_15ms() {
		bullet::tick();
		//if(plr::ms_registration() == MS_15) {
		//	if(plr::should_fire()) {
		//		plr::fire_weapon();
		//	}
		//}
	}
	void always_50ms() {
		//if(plr::ms_registration() == MS_50) {
		//	if(plr::should_fire()) {
		//		plr::fire_weapon();
		//	}
		//}
	}
	void always_250ms() {
		//if(plr::ms_registration() == MS_250) {
		//	if(plr::should_fire()) {
		//		plr::fire_weapon();
		//	}
		//}
	}
	void always_500ms() {
		//if(plr::ms_registration() == MS_500) {
		//	if(plr::should_fire()) {
		//		plr::fire_weapon();
		//	}
		//}

	}
	void always_1sec() {
		bullet::tick();
		//if(plr::ms_registration() == SEC_1) {
		//	if(plr::should_fire()) {
		//		plr::fire_weapon();
		//	}
		//}

	}
	void tick() {
		m_15ms_end = m_5ms_end = m_2ms_end = m_1sec_end = m_500ms_end = m_50ms_end = m_250ms_end = m_10ms_end = m_end = clk::now();
		diff = std::chrono::duration_cast< std::chrono::milliseconds >(m_end - m_start);
		diff_2ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_2ms_end- m_2ms_start);
		diff_5ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_5ms_end- m_5ms_start);
		diff_10ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_10ms_end- m_10ms_start);
		diff_15ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_15ms_end- m_15ms_start);
		diff_50ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_50ms_end- m_50ms_start);
		diff_250ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_250ms_end- m_250ms_start);
		diff_500ms = std::chrono::duration_cast< std::chrono::milliseconds >(m_500ms_end- m_500ms_start);
		diff_1sec = std::chrono::duration_cast< std::chrono::milliseconds >(m_1sec_end- m_1sec_start);
		if(plr::should_fire()) {
			plr::fire_weapon();
		}
		bullet::tick();

		if(diff_2ms.count() >= 2) {
			m_2ms_start = m_2ms_end;
			dispatch_slice(2);
			always_2ms();
		}
		if(diff_5ms.count() >= 5) {
			m_5ms_start = m_5ms_end;
			dispatch_slice(5);
			always_5ms();
		}
		if(diff_10ms.count() >= 10) {
			m_10ms_start = m_10ms_end;
			dispatch_slice(10);
			always_10ms();
		}
		if(diff_15ms.count() >= 15) {
			m_15ms_start = m_15ms_end;
			dispatch_slice(15);
			always_15ms();
		}
		if(diff_50ms.count() >= 50) {
			m_50ms_start = m_50ms_end;
			dispatch_slice(50);
			always_50ms();
		}
		if(diff_250ms.count() >= 250) {
			m_250ms_start = m_250ms_end;
			dispatch_slice(250);
			always_250ms();
		}
		if(diff_500ms.count() >= 500) {
			m_500ms_start = m_500ms_end;
			dispatch_slice(500);
			always_500ms();
		}
		if(diff_1sec.count() >= 1000) {
			m_1sec_start = m_1sec_end;
			dispatch_slice(1000);
			always_1sec();
		}
	}

};
