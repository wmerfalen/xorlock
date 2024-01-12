#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "timeline.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "graphical-decay.hpp"
#include <sys/time.h>
#include "sound/gunshot.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[TIMELINE][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[TIMELINE][ERROR]: " << A << "\n";
extern Mix_Chunk* mp5_shot;
extern std::array<Mix_Chunk*,wpn::weapon_t::WPN_MAX_SIZE> weapon_waves;
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
    m_decay_index = 0;
    for(std::size_t i=0; i < GR_DECAY_SIZE; ++i) {
      m_decay_list[i].run_me = false;
    }
  }
  uint64_t play_cycle_at = 0;
  void tick() {
    static Player* p = plr::get();

    if(play_cycle_at && tick::get() >= play_cycle_at){
      m_debug("play_spas12_cycle!!!");
      sound::play_spas12_cycle();
      play_cycle_at = 0;
    }
    if(!p->ammo && !dbg::unlimited_ammo()){
      return;
    }
    auto ammo = *p->ammo;
    if(dbg::unlimited_ammo()){
      ammo = 99;
    }
    if(p->firing_weapon && p->weapon_should_fire()){
      if(ammo){
        sound::play_weapon(p->equipped_weapon);
        bullet::queue_bullets(p->weapon_stats());
        p->consume_ammo();
        if(is_shotgun(p->equipped_weapon)){
          play_cycle_at = tick::get() + (p->primary->stat(WPN_COOLDOWN_BETWEEN_SHOTS) * 0.30);
          bullet::draw_shell_at(p->cx,p->cy,p->primary->stat(WPN_TYPE));
        }
      }else{
        p->weapon_click();
      }
#ifndef WEAPON_SHOULD_ALWAYS_FIRE
      if(p->weapon_is_semi_auto()){
        p->firing_weapon = false;
      }
#endif
    }
  }

  timeval start,stop;
  void start_timer() {
    gettimeofday(&start,nullptr);
  }
  uint64_t stop_timer() {
    gettimeofday(&stop,nullptr);
    return stop.tv_usec - start.tv_usec;
  }





};
