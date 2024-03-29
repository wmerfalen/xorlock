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
#include "sound/reload.hpp"
#include "reload.hpp"

//#define RELOAD_DEBUG
#ifdef RELOAD_DEBUG
#define m_debug(A) std::cerr << "[DEBUG]: " << __FILE__ << ":" << __LINE__ << "[" << __FUNCTION__ << "]->" << A << "\n";
#else
#define m_debug(A)
#endif
namespace reload {
  ReloadManager::ReloadManager(
      uint32_t* clip_size,
      uint16_t* ammo,
      uint16_t* total_ammo,
      weapon_stats_t* wpn_stats) :
    m_clip_size(clip_size),
    m_ammo(ammo),
    m_total_ammo(total_ammo),
    m_reloading(false),
    m_state(reload_phase_t::IDLE),
    m_response(reload_response_t::DONE),
    m_weapon_stats(wpn_stats) {
      m_is_frag = false;
      m_start_reload_tick = 0;
      m_is_performing_rolling_reload = false;
    }
  bool ReloadManager::clip_full() const {
    return *m_ammo == *m_clip_size;
  }
  bool ReloadManager::not_enough_ammo() const {
    return *m_total_ammo == 0;
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
    if((*m_ammo) > (*m_clip_size)){
      m_response = reload_response_t::CLIP_FULL;
      return m_response;
    }
    if(is_shotgun((*m_weapon_stats)[WPN_TYPE])){
      m_state = reload_phase_t::EJECTING_MAG;
      m_response = reload_response_t::STARTING_RELOAD;
      m_start_reload_tick = tick::get();
      m_is_performing_rolling_reload = true;
      return m_response;
    }
    m_start_reload_tick = tick::get();
    m_state = reload_phase_t::EJECTING_MAG;
    m_pull_slide = m_ammo == 0;
    m_response = reload_response_t::STARTING_RELOAD;
    sound::reload::play_eject();
    return m_response;
  }
  void ReloadManager::stop_rolling_reload(){
    m_is_performing_rolling_reload = false;
  }
  bool ReloadManager::can_reload() {
    return !is_reloading() && *m_total_ammo > 0;
  }
  bool ReloadManager::is_reloading() const {
    return m_state > reload_phase_t::IDLE && m_state != reload_phase_t::RELOAD_DONE;
  }
  const reload_phase_t& ReloadManager::state() const {
    return m_state;
  }
  bool ReloadManager::supports_rolling_reload() const {
    return is_shotgun((*m_weapon_stats)[WPN_TYPE]);
  }

  bool ReloadManager::is_performing_rolling_reload() const {
    return m_is_performing_rolling_reload;
  }
  const reload_phase_t& ReloadManager::tick() {
    const uint64_t& tick = tick::get();
    static constexpr uint64_t mult = 1;
    if(is_shotgun((*m_weapon_stats)[WPN_TYPE]) && m_is_performing_rolling_reload){
      uint64_t stat = (*m_weapon_stats)[WPN_PULL_REPLACEMENT_MAG_TICKS];
      if(m_start_reload_tick + (mult * stat) <= tick) {
        if(m_state != reload_phase_t::PULLING_REPLACEMENT_MAG){
          sound::reload::play_pull_replacement_mag();
          m_state = reload_phase_t::PULLING_REPLACEMENT_MAG;
        }
      }
      if(m_start_reload_tick + (2 * stat) <= tick){
        if(m_state == reload_phase_t::PULLING_REPLACEMENT_MAG){
          load_mag();
          m_state = reload_phase_t::RELOAD_DONE;
          m_start_reload_tick = tick::get();
          if((*m_ammo) == (*m_clip_size)){
            stop_rolling_reload();
          }
        }
      }
      return m_state;
    }
    if(m_is_frag == false){
      uint64_t stat = (*m_weapon_stats)[WPN_MAG_EJECT_TICKS];
      if(m_start_reload_tick + (mult * stat) >= tick) {
        m_state = reload_phase_t::EJECTING_MAG;
        return m_state;
      }
      stat += (*m_weapon_stats)[WPN_PULL_REPLACEMENT_MAG_TICKS];
      if(m_start_reload_tick + (mult * stat) >= tick) {
        if(m_state != reload_phase_t::PULLING_REPLACEMENT_MAG){
          sound::reload::play_pull_replacement_mag();
        }
        m_state = reload_phase_t::PULLING_REPLACEMENT_MAG;
        return m_state;
      }
      stat += (*m_weapon_stats)[WPN_LOADING_MAG_TICKS];
      if(m_start_reload_tick + (mult * stat) >= tick) {
        if(m_state != reload_phase_t::LOADING_MAG){
          sound::reload::play_load_mag();
        }
        m_state = reload_phase_t::LOADING_MAG;
        return m_state;
      }
      stat += (*m_weapon_stats)[WPN_SLIDE_PULL_TICKS];
      if(m_pull_slide && m_start_reload_tick + (mult * stat) >= tick) {
        if(m_state != reload_phase_t::PULLING_SLIDE){
          sound::reload::play_weapon_slide();
        }
        m_state = reload_phase_t::PULLING_SLIDE;
        return m_state;
      }
      if(is_reloading()) {
        load_mag();
        m_state = reload_phase_t::IDLE;
      }
      return m_state;
    }else{
      return m_state = reload_phase_t::IDLE;
    }
  }
  void ReloadManager::load_mag() {
    if(m_is_frag){
      return;
    }
    if(is_shotgun((*m_weapon_stats)[WPN_TYPE])){
      if((*m_ammo) < (*m_clip_size) && (*m_total_ammo)){
        (*m_ammo) += 1;
        (*m_total_ammo) -= 1;
        if((*m_ammo) == (*m_clip_size)){
          stop_rolling_reload();
          m_state = reload_phase_t::LOADED;
        }
        return;
      }
      m_state = reload_phase_t::IDLE;
      return;
    }

    for(; *m_ammo < *m_clip_size && *m_total_ammo > 0; ++(*m_ammo),--(*m_total_ammo)) {
    }
    m_state = reload_phase_t::LOADED;
  }
  void ReloadManager::update(uint32_t* clip_size,
      uint16_t* ammo,
      uint16_t* total_ammo,
      weapon_stats_t* wpn_stats){
    m_clip_size = clip_size;
    m_ammo = ammo;
    m_total_ammo = total_ammo;
    m_reloading = false;
    m_state = reload_phase_t::IDLE;
    m_response = reload_response_t::DONE;
    m_weapon_stats = wpn_stats;
    m_is_frag = false;
  }
  ReloadManager::ReloadManager(){
    m_is_frag = false;
    m_clip_size = nullptr;
    m_ammo = nullptr;
    m_total_ammo = nullptr;
    m_reloading = false;
    m_state = reload_phase_t::IDLE;
    m_response = reload_response_t::DONE;
    m_weapon_stats = nullptr;
    m_start_reload_tick = 0;
    stop_rolling_reload();
  }
  void ReloadManager::update_frag(uint32_t* clip_size,
      uint16_t* ammo,
      uint16_t* total_ammo,
      explosive_stats_t* exp_stats){
    m_clip_size = clip_size;
    m_ammo = ammo;
    m_total_ammo = total_ammo;
    m_reloading = false;
    m_state = reload_phase_t::IDLE;
    m_response = reload_response_t::DONE;
    m_explosive_stats = exp_stats;
    m_is_frag = true;
  }
  void init() {

  }
};
