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
  void tick() {
    static Player* p = plr::get();

    auto ammo = *p->ammo;
    bool queue = false;
    if(p->firing_weapon){
      if(p->primary_equipped && p->mp5 && p->mp5->should_fire()){
        queue = true;
        if(ammo){
          sound::play_mp5_gunshot();
        }
      }else if(p->secondary_equipped && p->p226 && p->p226->should_fire()){
        queue = true;
        if(ammo){
          sound::play_p226_gunshot();
        }
      }
      if(queue) {
        if(ammo){
          bullet::queue_bullets(p->weapon_stats());
          p->consume_ammo();
        }else{
          p->weapon_click();
        }
      }
    }
    bullet::tick();
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
