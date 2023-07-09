#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "player.hpp"
#include "weapons.hpp"
#include "direction.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include <map>

#include "circle.hpp"
#include "npc-spetsnaz.hpp"
#include "weapons/smg/mp5.hpp"
#include "cursor.hpp"
#include "bullet.hpp"
#include "draw.hpp"
#include "font.hpp"
#include "colors.hpp"
#include "draw-state/player.hpp"
#include "draw-state/reticle.hpp"
#include "reload.hpp"

//#define RELOAD_DEBUG
#ifdef RELOAD_DEBUG
#define m_debug(A) std::cerr << "[DEBUG]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";
#else
#define m_debug(A)
#endif
namespace reload {
	ReloadManager::ReloadManager(
	    uint32_t& clip_size,
	    uint16_t& ammo,
	    uint16_t& total_ammo,
	    const weapon_stats_t& wpn_stats) :
		m_clip_size(clip_size),
		m_ammo(ammo),
		m_total_ammo(total_ammo),
		m_reloading(false),
		m_state(reload_phase_t::IDLE),
		m_response(reload_response_t::DONE),
		m_weapon_stats(wpn_stats) {
	}
	bool ReloadManager::clip_full() const {
		return m_ammo == m_clip_size;
	}
	bool ReloadManager::not_enough_ammo() const {
		return m_total_ammo == 0;
	}
	const reload_response_t& ReloadManager::start_reload() {
		if(is_reloading()) {
			m_response = reload_response_t::CURRENTLY_RELOADING;
			return m_response;
		}
		if(clip_full()) {
			m_response = reload_response_t::CLIP_FULL;
			return m_response;
		}
		if(not_enough_ammo()) {
			m_response = reload_response_t::NOT_ENOUGH_AMMO;
			return m_response;
		}
		m_state = reload_phase_t::EJECTING_MAG;
		m_pull_slide = m_ammo == 0;
		m_response = reload_response_t::STARTING_RELOAD;
		m_start_reload_tick = tick::get();
		return m_response;
	}
	bool ReloadManager::can_reload() {
		return !is_reloading() && m_total_ammo > 0;
	}
	bool ReloadManager::is_reloading() const {
		return m_state > reload_phase_t::IDLE && m_state != reload_phase_t::RELOAD_DONE;
	}
	const reload_phase_t& ReloadManager::state() const {
		return m_state;
	}

	const reload_phase_t& ReloadManager::tick() {
		const uint64_t& tick = tick::get();
		static constexpr uint64_t mult = 1;
		uint64_t stat = m_weapon_stats[WPN_MAG_EJECT_TICKS];
		if(m_start_reload_tick + (mult * stat) >= tick) {
			m_state = reload_phase_t::EJECTING_MAG;
			return m_state;
		}
		stat += m_weapon_stats[WPN_PULL_REPLACEMENT_MAG_TICKS];
		if(m_start_reload_tick + (mult * stat) >= tick) {
			m_state = reload_phase_t::PULLING_REPLACEMENT_MAG;
			return m_state;
		}
		stat += m_weapon_stats[WPN_LOADING_MAG_TICKS];
		if(m_start_reload_tick + (mult * stat) >= tick) {
			m_state = reload_phase_t::LOADING_MAG;
			return m_state;
		}
		stat += m_weapon_stats[WPN_SLIDE_PULL_TICKS];
		if(m_pull_slide && m_start_reload_tick + (mult * stat) >= tick) {
			m_state = reload_phase_t::PULLING_SLIDE;
			return m_state;
		}
		if(is_reloading()) {
			load_mag();
			m_state = reload_phase_t::IDLE;
		}
		return m_state;
	}
	void ReloadManager::load_mag() {
		for(; m_ammo < m_clip_size && m_total_ammo > 0; ++m_ammo,--m_total_ammo) {
		}
		m_state = reload_phase_t::LOADED;
	}
	void init() {

	}
};
