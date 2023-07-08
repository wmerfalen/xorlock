#ifndef __RELOAD_HEADER__
#define __RELOAD_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "actor.hpp"
#include "world.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "bullet-pool.hpp"
#include <map>

#include "circle.hpp"
#include "draw.hpp"
#include "weapons.hpp"
#include "player.hpp"
#include <memory>

void save_draw_color();
void restore_draw_color();
void set_draw_color(const char* s);

namespace reload {
	enum reload_response_t : uint16_t {
    NOT_ENOUGH_AMMO,
    CLIP_FULL,
    CURRENTLY_RELOADING,
    STARTING_RELOAD,
    DONE,
	};

  enum reload_phase_t : uint16_t {
    IDLE = 0,
    EJECTING_MAG = 1,
    PULLING_REPLACEMENT_MAG,
    LOADING_MAG,
    LOADED,
    PULLING_SLIDE,
    RELOAD_DONE,
  };
	struct ReloadManager {
		ReloadManager() = delete;
    ReloadManager(
        uint32_t& clip_size,
        uint16_t& ammo,
        uint16_t& total_ammo,
        const weapon_stats_t& wpn_stats);
		/** Copy constructor */
		ReloadManager(const ReloadManager& other) = delete;

    bool can_reload();
    bool is_reloading() const;
    bool clip_full() const;
    bool not_enough_ammo() const;
    const reload_phase_t& state() const;
    const reload_response_t& start_reload();

    const reload_phase_t& tick();

    private:
    void load_mag();
    uint32_t& m_clip_size;
    uint16_t& m_ammo;
    uint16_t& m_total_ammo;
		uint32_t m_reload_ticks;
    bool m_reloading;
    reload_phase_t m_state;
    reload_response_t m_response;
    bool m_pull_slide;
    const weapon_stats_t& m_weapon_stats;
    uint64_t m_start_reload_tick;
	};
	void init();
};
#endif
