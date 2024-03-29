#include "loot.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "filesystem.hpp"
#include "weapons/pistol/p226.hpp"
#include "weapons.hpp"
#include "player.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[LOOT][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[LOOT][ERROR]: " << A << "\n";

namespace loot {
  // TODO: flesh this out
  struct loot_descriptor_t {
    uint32_t id;
    float drop_rate;
    std::string name;
  };
  static std::forward_list<std::unique_ptr<Loot>> loot;
  static std::vector<Loot*> loot_list;
  static SDL_mutex* loot_list_mutex = SDL_CreateMutex();
  static uint64_t loot_id = 0;
  static FILE* loot_fp = nullptr;
  void flush_id(){
    FILE* fp = fopen(constants::loot_id_file,"w+");
    fwrite(std::to_string(loot_id).c_str(),sizeof(char),std::to_string(loot_id).length(),fp);
    fwrite("\n",sizeof(char),1,fp);
    fclose(fp);
  }
  uint64_t loot_next_id(){
    ++loot_id;
    flush_id();
    return loot_id;
  }
  void init(){
    m_debug("loot init");
    m_debug("sizeof ExportWeapon: " << sizeof(ExportWeapon));
    m_debug("sizeof ExportGrenade: " << sizeof(ExportGrenade));
    if(!fs::exists(constants::loot_id_file)){
      m_debug("loot id file doesnt exist... creating...");
      loot_id = 0;
      flush_id();
    }
    std::string guts;
    std::string error;
    int i = fs::file_get_contents(constants::loot_id_file, guts,error);
    if(i < 0){
      m_error("Couldn't get file contents of loot id file: '" << constants::loot_id_file << "'");
      return;
    }
    std::string num;
    for(const auto& ch : guts){
      if(isdigit(ch)){
        num += ch;
        continue;
      }
      if(ch == '\n'){
        break;
      }
    }
    loot_id = atol(num.c_str());
    m_debug("loot_id: " << loot_id);
  }

  void tick(){
    SDL_Rect r;
    r.w = 40;
    r.h = 40;
    LOCK_MUTEX(loot_list_mutex);
    for(const auto& l : loot_list){
      r.x = l->self.rect.x;
      r.y = l->self.rect.y;
      draw::blatant_rect(&r);
    }
    UNLOCK_MUTEX(loot_list_mutex);
  }
  void program_exit(){
    flush_id();
  }
  void load_tiers(){
    // TODO: load tiered randomized weapon data
  }
  void do_bonuses(weapon_stats_t* drop_stats){
    for(const auto& field : {WPN_RELOAD_TM,
        WPN_COOLDOWN_BETWEEN_SHOTS,
        WPN_MAG_EJECT_TICKS,
        WPN_PULL_REPLACEMENT_MAG_TICKS,
        WPN_LOADING_MAG_TICKS,
        WPN_SLIDE_PULL_TICKS,
        WPN_ACCURACY_DEVIATION_START,
        WPN_ACCURACY_DEVIATION_END,
        WPN_WIELD_TICKS}){
      if(rng::chance(10)){
        auto current = (*drop_stats)[field];
        auto buff = rand_between(current / 3, current / 2);
        if(current - buff > 0){
          m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
          (*drop_stats)[field] -= buff;
        }
      }
      if((*drop_stats)[WPN_ACCURACY_DEVIATION_END] <= (*drop_stats)[WPN_ACCURACY_DEVIATION_START]){
        (*drop_stats)[WPN_ACCURACY_DEVIATION_START] /= 2;
      }
    }
    for(const auto& field : {WPN_DMG_LO,
        WPN_DMG_HI,
        WPN_CLIP_SZ,
        WPN_AMMO_MX,
        WPN_ACCURACY}){
      if(rng::chance(10)){
        auto current = (*drop_stats)[field];
        auto buff = rand_between(current * 2, current * 3);
        if(current + buff > 0){
          m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
          (*drop_stats)[field] += buff;
        }
      }
    }
  }
  void Loot::handle_bomber(const int& npc_id){
    id = loot_next_id();
    loot_id = id;

    switch(rand_between(1,4) * -1){
      case -1: {
                 m_debug("bomber. dropping pistol only");
                 object_type = type_t::GUN;
                 type = type_t::GUN;
                 item_type = wpn::weapon_type_t::WPN_T_PISTOL;
                 weapon_stats_t drop_stats;
                 drop_stats[WPN_FLAGS] = 0;
                 drop_stats[WPN_TYPE] = random_pistol_type();
                 name = weapon_name(&drop_stats);
                 drop_stats[WPN_DMG_LO] = rand_between(15,95);
                 drop_stats[WPN_DMG_HI] = rand_between(100,155);
                 drop_stats[WPN_BURST_DLY] = 3;
                 drop_stats[WPN_PIXELS_PT] = 38;
                 drop_stats[WPN_CLIP_SZ] = rand_between(25,55);
                 drop_stats[WPN_AMMO_MX] = drop_stats[WPN_CLIP_SZ] * rand_between(4,10);
                 drop_stats[WPN_RELOAD_TM] = rand_between(1000,5000);
                 drop_stats[WPN_COOLDOWN_BETWEEN_SHOTS] = rand_between(170,280);
                 //drop_stats[WPN_MS_REGISTRATION] = 0;
                 drop_stats[WPN_MAG_EJECT_TICKS] = rand_between(250,800);
                 drop_stats[WPN_PULL_REPLACEMENT_MAG_TICKS] = rand_between(250,550);
                 drop_stats[WPN_LOADING_MAG_TICKS] = rand_between(250,800);
                 drop_stats[WPN_SLIDE_PULL_TICKS] = rand_between(250,550);
                 drop_stats[WPN_WIELD_TICKS] = rand_between(200,800);
                 drop_stats[WPN_ACCURACY] = rand_between(1,100); // TODO: limit by player level
                 drop_stats[WPN_ACCURACY_DEVIATION_START] = rand_between(1,11);
                 drop_stats[WPN_ACCURACY_DEVIATION_END] = rand_between(drop_stats[WPN_ACCURACY_DEVIATION_START],drop_stats[WPN_ACCURACY_DEVIATION_START] * 1.5);
                 do_bonuses(&drop_stats);
                 wpn_debug::dump(&drop_stats);
                 stats.emplace<0>(drop_stats);
                 write_to_file();
                 return;
               }
      case -2:{
                m_debug("bomber. dropping mp5 only");
                object_type = type_t::GUN;
                name = "mp5"; // TODO: randomize this
                              // Only drop pistols
                type = type_t::GUN;
                item_type = wpn::weapon_type_t::WPN_T_SMG; // TODO: randomize this
                weapon_stats_t drop_stats;
                drop_stats[WPN_FLAGS] = 0;
                drop_stats[WPN_TYPE] = wpn::weapon_t::WPN_MP5;
                drop_stats[WPN_DMG_LO] = rand_between(15,95);
                drop_stats[WPN_DMG_HI] = rand_between(100,155);
                drop_stats[WPN_BURST_DLY] = 3;
                drop_stats[WPN_PIXELS_PT] = 38;
                drop_stats[WPN_CLIP_SZ] = rand_between(35,75);
                drop_stats[WPN_AMMO_MX] = drop_stats[WPN_CLIP_SZ] * rand_between(4,10);
                drop_stats[WPN_RELOAD_TM] = rand_between(1000,3000);
                drop_stats[WPN_COOLDOWN_BETWEEN_SHOTS] = rand_between(80,120);
                //drop_stats[WPN_MS_REGISTRATION] = 0;
                drop_stats[WPN_MAG_EJECT_TICKS] = rand_between(250,800);
                drop_stats[WPN_PULL_REPLACEMENT_MAG_TICKS] = rand_between(250,550);
                drop_stats[WPN_LOADING_MAG_TICKS] = rand_between(250,800);
                drop_stats[WPN_SLIDE_PULL_TICKS] = rand_between(250,550);
                drop_stats[WPN_WIELD_TICKS] = rand_between(200,800);
                drop_stats[WPN_ACCURACY] = rand_between(1,100); // TODO: limit by player level
                drop_stats[WPN_ACCURACY_DEVIATION_START] = rand_between(2,10);
                drop_stats[WPN_ACCURACY_DEVIATION_END] = rand_between(drop_stats[WPN_ACCURACY_DEVIATION_START],drop_stats[WPN_ACCURACY_DEVIATION_START] * 1.20);
                do_bonuses(&drop_stats);
                for(const auto& field : {WPN_RELOAD_TM,
                    WPN_COOLDOWN_BETWEEN_SHOTS,
                    WPN_MAG_EJECT_TICKS,
                    WPN_PULL_REPLACEMENT_MAG_TICKS,
                    WPN_LOADING_MAG_TICKS,
                    WPN_SLIDE_PULL_TICKS,
                    WPN_WIELD_TICKS}){
                  if(rng::chance(10)){
                    auto current = drop_stats[field];
                    auto buff = rand_between(current / 3, current / 2);
                    if(current - buff > 0){
                      m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
                      drop_stats[field] -= buff;
                    }
                  }
                }
                wpn_debug::dump(&drop_stats);
                stats.emplace<0>(drop_stats);
                write_to_file();
                return;
              }
      case -3:{
                m_debug("bomber. dropping explosive only");
                object_type = type_t::EXPLOSIVE;
                name = "frag";
                type = type_t::EXPLOSIVE;
                item_type = wpn::grenade_type_t::GREN_T_FRAG; // TODO: randomize this
                explosive_stats_t drop_stats;
                //GREN_T_FRAG,
                //GREN_T_STUN,
                //GREN_T_SMOKE,
                //GREN_T_INCENDIARY,
                //drop_stats[WPN_FLAGS] = 0;
                drop_stats[EXP_FLAGS] = 0;
                drop_stats[EXP_TYPE] = wpn::grenade_type_t::GREN_T_FRAG; // TODO: randomize this
                drop_stats[EXP_DMG_LO] = rand_between(15,95); // FIXME: find preferred values
                drop_stats[EXP_DMG_HI] = rand_between(100,155); // FIXME: find preferred values
                drop_stats[EXP_PULL_PIN_TICKS] = rand_between(1800,8000); // FIXME: find preferred values
                drop_stats[EXP_RADIUS] = rand_between(150,950); // FIXME: find preferred values
                for(const auto& field : {
                    EXP_PULL_PIN_TICKS,}){
                  if(rng::chance(10)){
                    auto current = drop_stats[field];
                    auto buff = rand_between(current / 3, current / 2); // FIXME: find preferred values
                    if(current - buff > 0){
                      m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
                      drop_stats[field] -= buff;
                    }
                  }
                }
                for(const auto& field : {
                    EXP_DMG_LO,
                    EXP_DMG_HI,
                    EXP_RADIUS,
                    }){
                  if(rng::chance(10)){
                    auto current = drop_stats[field];
                    auto buff = rand_between(current / 8, current / 4); // FIXME: find preferred values
                    m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
                    drop_stats[field] += buff;
                  }
                }
                wpn_debug::dump(&drop_stats);
                stats.emplace<1>(drop_stats);
                write_to_file();
                return;
              }
      case -4:{
                m_debug("bomber. dropping shotgun only");
                object_type = type_t::GUN;
                name = "SPAS-12"; // TODO: randomize this
                                  // drop shotgun
                type = type_t::GUN;
                item_type = wpn::weapon_type_t::WPN_T_SHOTGUN;
                weapon_stats_t drop_stats;
                drop_stats[WPN_FLAGS] = 0;
                drop_stats[WPN_TYPE] = wpn::weapon_t::WPN_SPAS12; // TODO: randomize this
                                                                  // TODO: find optimal ranges for all of these
                drop_stats[WPN_DMG_LO] = rand_between(35,195);
                drop_stats[WPN_DMG_HI] = rand_between(200,355);
                drop_stats[WPN_BURST_DLY] = 0;
                drop_stats[WPN_PIXELS_PT] = 44;
                drop_stats[WPN_CLIP_SZ] = rand_between(8,14);
                drop_stats[WPN_AMMO_MX] = drop_stats[WPN_CLIP_SZ] * rand_between(4,10);
                drop_stats[WPN_RELOAD_TM] = rand_between(500,1500);
                drop_stats[WPN_COOLDOWN_BETWEEN_SHOTS] = rand_between(580,1550);
                //drop_stats[WPN_MS_REGISTRATION] = 0;
                drop_stats[WPN_MAG_EJECT_TICKS] = rand_between(110,300);
                drop_stats[WPN_PULL_REPLACEMENT_MAG_TICKS] = rand_between(250,550);
                drop_stats[WPN_LOADING_MAG_TICKS] = rand_between(250,800);
                drop_stats[WPN_SLIDE_PULL_TICKS] = rand_between(250,550);
                drop_stats[WPN_WIELD_TICKS] = rand_between(200,400);
                if(rng::chance(3)){
                  drop_stats[WPN_ACCURACY] = 100;
                  drop_stats[WPN_ACCURACY_DEVIATION_START] = 0;
                  drop_stats[WPN_ACCURACY_DEVIATION_END] = 0;
                }else{
                  int dev_start,dev_end;
                  do {
                    dev_start = rand_between(3,6);
                    dev_end = rand_between(dev_start,dev_start * 2);
                    drop_stats[WPN_ACCURACY] = rand_between(1,100); // TODO: limit by player level
                    if(rng::chance(20)){
                      dev_end -= rand_between(1,2);
                    }
                    if(rng::chance(40)){
                      dev_start -= rand_between(1,2);
                    }
                  }while(dev_end < dev_start);
                  drop_stats[WPN_ACCURACY_DEVIATION_START] = dev_start;
                  drop_stats[WPN_ACCURACY_DEVIATION_END] = dev_end;
                }
                do_bonuses(&drop_stats);
                for(const auto& field : {WPN_RELOAD_TM,
                    WPN_COOLDOWN_BETWEEN_SHOTS,
                    WPN_MAG_EJECT_TICKS,
                    WPN_PULL_REPLACEMENT_MAG_TICKS,
                    WPN_LOADING_MAG_TICKS,
                    WPN_SLIDE_PULL_TICKS,
                    WPN_WIELD_TICKS,
                    WPN_ACCURACY_DEVIATION_START,
                    WPN_ACCURACY_DEVIATION_END,}){
                  if(rng::chance(10)){
                    auto current = drop_stats[field];
                    auto buff = rand_between(current / 3, current / 2);
                    if(current - buff > 0){
                      m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
                      drop_stats[field] -= buff;
                    }
                  }
                }
                wpn_debug::dump(&drop_stats);
                stats.emplace<0>(drop_stats);
                write_to_file();
                return;
              }
    }
  }
  void Loot::handle_spetsnaz(const int& npc_id){
    id = loot_next_id();
    loot_id = id;
    switch(rand_between(1,4) * -1){
      case -1: {
                 m_debug("SPETSNAZ. dropping pistol only");
                 object_type = type_t::GUN;
                 type = type_t::GUN;
                 item_type = wpn::weapon_type_t::WPN_T_PISTOL;
                 weapon_stats_t drop_stats;
                 drop_stats[WPN_FLAGS] = 0;
                 drop_stats[WPN_TYPE] = random_pistol_type();
                 name = weapon_name(&drop_stats);
                 drop_stats[WPN_DMG_LO] = rand_between(15,95);
                 drop_stats[WPN_DMG_HI] = rand_between(100,155);
                 drop_stats[WPN_BURST_DLY] = 3;
                 drop_stats[WPN_PIXELS_PT] = 38;
                 drop_stats[WPN_CLIP_SZ] = rand_between(25,55);
                 drop_stats[WPN_AMMO_MX] = drop_stats[WPN_CLIP_SZ] * rand_between(4,10);
                 drop_stats[WPN_RELOAD_TM] = rand_between(1000,5000);
                 drop_stats[WPN_COOLDOWN_BETWEEN_SHOTS] = rand_between(170,280);
                 //drop_stats[WPN_MS_REGISTRATION] = 0;
                 drop_stats[WPN_MAG_EJECT_TICKS] = rand_between(250,800);
                 drop_stats[WPN_PULL_REPLACEMENT_MAG_TICKS] = rand_between(250,550);
                 drop_stats[WPN_LOADING_MAG_TICKS] = rand_between(250,800);
                 drop_stats[WPN_SLIDE_PULL_TICKS] = rand_between(250,550);
                 drop_stats[WPN_WIELD_TICKS] = rand_between(200,800);
                 drop_stats[WPN_ACCURACY] = rand_between(1,100); // TODO: limit by player level
                 drop_stats[WPN_ACCURACY_DEVIATION_START] = rand_between(1,11);
                 drop_stats[WPN_ACCURACY_DEVIATION_END] = rand_between(drop_stats[WPN_ACCURACY_DEVIATION_START],drop_stats[WPN_ACCURACY_DEVIATION_START] * 1.5);
                 do_bonuses(&drop_stats);
                 wpn_debug::dump(&drop_stats);
                 stats.emplace<0>(drop_stats);
                 write_to_file();
                 return;
               }
      case -2: {
                 m_debug("SPETSNAZ. dropping mp5 only");
                 object_type = type_t::GUN;
                 name = "mp5"; // TODO: randomize this
                               // Only drop pistols
                 type = type_t::GUN;
                 item_type = wpn::weapon_type_t::WPN_T_SMG; // TODO: randomize this
                 weapon_stats_t drop_stats;
                 drop_stats[WPN_FLAGS] = 0;
                 drop_stats[WPN_TYPE] = wpn::weapon_t::WPN_MP5;
                 drop_stats[WPN_DMG_LO] = rand_between(15,95);
                 drop_stats[WPN_DMG_HI] = rand_between(100,155);
                 drop_stats[WPN_BURST_DLY] = 3;
                 drop_stats[WPN_PIXELS_PT] = 38;
                 drop_stats[WPN_CLIP_SZ] = rand_between(35,75);
                 drop_stats[WPN_AMMO_MX] = drop_stats[WPN_CLIP_SZ] * rand_between(4,10);
                 drop_stats[WPN_RELOAD_TM] = rand_between(1000,3000);
                 drop_stats[WPN_COOLDOWN_BETWEEN_SHOTS] = rand_between(80,120);
                 //drop_stats[WPN_MS_REGISTRATION] = 0;
                 drop_stats[WPN_MAG_EJECT_TICKS] = rand_between(250,800);
                 drop_stats[WPN_PULL_REPLACEMENT_MAG_TICKS] = rand_between(250,550);
                 drop_stats[WPN_LOADING_MAG_TICKS] = rand_between(250,800);
                 drop_stats[WPN_SLIDE_PULL_TICKS] = rand_between(250,550);
                 drop_stats[WPN_WIELD_TICKS] = rand_between(200,800);
                 drop_stats[WPN_ACCURACY] = rand_between(1,100); // TODO: limit by player level
                 drop_stats[WPN_ACCURACY_DEVIATION_START] = rand_between(2,10);
                 drop_stats[WPN_ACCURACY_DEVIATION_END] = rand_between(drop_stats[WPN_ACCURACY_DEVIATION_START],drop_stats[WPN_ACCURACY_DEVIATION_START] * 1.20);
                 do_bonuses(&drop_stats);
                 for(const auto& field : {WPN_RELOAD_TM,
                     WPN_COOLDOWN_BETWEEN_SHOTS,
                     WPN_MAG_EJECT_TICKS,
                     WPN_PULL_REPLACEMENT_MAG_TICKS,
                     WPN_LOADING_MAG_TICKS,
                     WPN_SLIDE_PULL_TICKS,
                     WPN_WIELD_TICKS}){
                   if(rng::chance(10)){
                     auto current = drop_stats[field];
                     auto buff = rand_between(current / 3, current / 2);
                     if(current - buff > 0){
                       m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
                       drop_stats[field] -= buff;
                     }
                   }
                 }
                 wpn_debug::dump(&drop_stats);
                 stats.emplace<0>(drop_stats);
                 write_to_file();
                 return;
               }
      case -3: {
                 m_debug("SPETSNAZ. dropping explosive only");
                 object_type = type_t::EXPLOSIVE;
                 name = "frag";
                 type = type_t::EXPLOSIVE;
                 item_type = wpn::grenade_type_t::GREN_T_FRAG; // TODO: randomize this
                 explosive_stats_t drop_stats;
                 //GREN_T_FRAG,
                 //GREN_T_STUN,
                 //GREN_T_SMOKE,
                 //GREN_T_INCENDIARY,
                 //drop_stats[WPN_FLAGS] = 0;
                 drop_stats[EXP_FLAGS] = 0;
                 drop_stats[EXP_TYPE] = wpn::grenade_type_t::GREN_T_FRAG; // TODO: randomize this
                 drop_stats[EXP_DMG_LO] = rand_between(15,95); // FIXME: find preferred values
                 drop_stats[EXP_DMG_HI] = rand_between(100,155); // FIXME: find preferred values
                 drop_stats[EXP_PULL_PIN_TICKS] = rand_between(1800,8000); // FIXME: find preferred values
                 drop_stats[EXP_RADIUS] = rand_between(150,950); // FIXME: find preferred values
                 for(const auto& field : {
                     EXP_PULL_PIN_TICKS,}){
                   if(rng::chance(10)){
                     auto current = drop_stats[field];
                     auto buff = rand_between(current / 3, current / 2); // FIXME: find preferred values
                     if(current - buff > 0){
                       m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
                       drop_stats[field] -= buff;
                     }
                   }
                 }
                 for(const auto& field : {
                     EXP_DMG_LO,
                     EXP_DMG_HI,
                     EXP_RADIUS,
                     }){
                   if(rng::chance(10)){
                     auto current = drop_stats[field];
                     auto buff = rand_between(current / 8, current / 4); // FIXME: find preferred values
                     m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
                     drop_stats[field] += buff;
                   }
                 }
                 wpn_debug::dump(&drop_stats);
                 stats.emplace<1>(drop_stats);
                 write_to_file();
                 return;
               }
      case -4: {
                 m_debug("SPETSNAZ. dropping shotgun only");
                 object_type = type_t::GUN;
                 name = "SPAS-12"; // TODO: randomize this
                                   // drop shotgun
                 type = type_t::GUN;
                 item_type = wpn::weapon_type_t::WPN_T_SHOTGUN;
                 weapon_stats_t drop_stats;
                 drop_stats[WPN_FLAGS] = 0;
                 drop_stats[WPN_TYPE] = wpn::weapon_t::WPN_SPAS12; // TODO: randomize this
                                                                   // TODO: find optimal ranges for all of these
                 drop_stats[WPN_DMG_LO] = rand_between(35,195);
                 drop_stats[WPN_DMG_HI] = rand_between(200,355);
                 drop_stats[WPN_BURST_DLY] = 0;
                 drop_stats[WPN_PIXELS_PT] = 44;
                 drop_stats[WPN_CLIP_SZ] = rand_between(8,14);
                 drop_stats[WPN_AMMO_MX] = drop_stats[WPN_CLIP_SZ] * rand_between(4,10);
                 drop_stats[WPN_RELOAD_TM] = rand_between(500,1500);
                 drop_stats[WPN_COOLDOWN_BETWEEN_SHOTS] = rand_between(580,1550);
                 //drop_stats[WPN_MS_REGISTRATION] = 0;
                 drop_stats[WPN_MAG_EJECT_TICKS] = rand_between(110,300);
                 drop_stats[WPN_PULL_REPLACEMENT_MAG_TICKS] = rand_between(250,550);
                 drop_stats[WPN_LOADING_MAG_TICKS] = rand_between(250,800);
                 drop_stats[WPN_SLIDE_PULL_TICKS] = rand_between(250,550);
                 drop_stats[WPN_WIELD_TICKS] = rand_between(200,400);
                 if(rng::chance(3)){
                   drop_stats[WPN_ACCURACY] = 100;
                   drop_stats[WPN_ACCURACY_DEVIATION_START] = 0;
                   drop_stats[WPN_ACCURACY_DEVIATION_END] = 0;
                 }else{
                   int dev_start,dev_end;
                   do {
                     dev_start = rand_between(3,6);
                     dev_end = rand_between(dev_start,dev_start * 2);
                     drop_stats[WPN_ACCURACY] = rand_between(1,100); // TODO: limit by player level
                     if(rng::chance(20)){
                       dev_end -= rand_between(1,2);
                     }
                     if(rng::chance(40)){
                       dev_start -= rand_between(1,2);
                     }
                   }while(dev_end < dev_start);
                   drop_stats[WPN_ACCURACY_DEVIATION_START] = dev_start;
                   drop_stats[WPN_ACCURACY_DEVIATION_END] = dev_end;
                 }
                 do_bonuses(&drop_stats);
                 for(const auto& field : {WPN_RELOAD_TM,
                     WPN_COOLDOWN_BETWEEN_SHOTS,
                     WPN_MAG_EJECT_TICKS,
                     WPN_PULL_REPLACEMENT_MAG_TICKS,
                     WPN_LOADING_MAG_TICKS,
                     WPN_SLIDE_PULL_TICKS,
                     WPN_WIELD_TICKS,
                     WPN_ACCURACY_DEVIATION_START,
                     WPN_ACCURACY_DEVIATION_END,}){
                   if(rng::chance(10)){
                     auto current = drop_stats[field];
                     auto buff = rand_between(current / 3, current / 2);
                     if(current - buff > 0){
                       m_debug("rng chance of 10 passed. buffing : " << weapon_slot_strings[field]);
                       drop_stats[field] -= buff;
                     }
                   }
                 }
                 wpn_debug::dump(&drop_stats);
                 stats.emplace<0>(drop_stats);
                 write_to_file();
                 return;
               }
    }
  }
  Loot::Loot(int npc_type,int npc_id,int in_cx,int in_cy){
    register_actor(&self);
    self.rect.x = in_cx;
    self.rect.y = in_cy;
    switch(npc_type){
      case constants::npc_type_t::NPC_BOMBER:
        handle_bomber(npc_id);
        break;
      default:
      case constants::npc_type_t::NPC_SPETSNAZ:
        handle_spetsnaz(npc_id);
        break;
    }
  }
  void Loot::write_to_file(){
    std::vector<char> buf;
    if(type == GUN){
      auto exp = export_weapon();
      buf.resize(sizeof(exp));
      std::string file = constants::loot_dir;
      file += "/";
      file += std::to_string(exp.id);

      FILE* fp = fopen(file.c_str(),"w+");
      fwrite((void*)&exp,sizeof(char),sizeof(exp),fp);
      fclose(fp);
    }else{
      auto exp = export_grenade();
      buf.resize(sizeof(exp));
      std::string file = constants::loot_dir;
      file += "/";
      file += std::to_string(exp.id);

      FILE* fp = fopen(file.c_str(),"w+");
      fwrite((void*)&exp,sizeof(char),sizeof(exp),fp);
      fclose(fp);
    }
  }
  ExportWeapon Loot::export_weapon(){
    ExportWeapon w;
    w.object_type = type_t::GUN;
    w.id = id;
    w.type = (wpn::weapon_type_t)item_type;
    w.stats = std::get<0>(stats);
    memset(&w.name[0],0,sizeof(w.name));
    bcopy(name.c_str(),&w.name[0],std::min(name.length(),sizeof(w.name)));
    return w;
  }
  ExportGrenade Loot::export_grenade(){
    ExportGrenade g;
    g.object_type = type_t::EXPLOSIVE;
    g.id = id;
    g.type = (wpn::grenade_type_t)item_type;
    g.stats = std::get<1>(stats);
    memset(&g.name[0],0,sizeof(g.name));
    bcopy(name.c_str(),&g.name[0],std::min(name.length(),sizeof(g.name)));
    return g;
  }

  void Loot::dump(){
    m_debug("id: " << id << "\nself.rect.x: " << self.rect.x << "\nself.rect.y: " << self.rect.y);
  }
  void dispatch(constants::npc_type_t npc_type, npc_id_t id, int in_cx, int in_cy){
    m_debug("npc died");
    LOCK_MUTEX(loot_list_mutex);
    auto ptr = std::make_unique<Loot>((int)npc_type,//int npc_type,
        (int)id, //int npc_id,
        (int)in_cx,//int cx, 
        (int)in_cy);
    loot_list.emplace_back(ptr.get());
    loot.emplace_front(std::move(ptr));
    UNLOCK_MUTEX(loot_list_mutex);
  }
  std::vector<Loot*> near_loot(SDL_Rect* r){
    std::vector<Loot*> nearby;
    SDL_Rect result;
    LOCK_MUTEX(loot_list_mutex);
    for(const auto& l : loot_list){
      SDL_Rect lr;
      lr.x = l->self.rect.x - 50;
      lr.y = l->self.rect.y - 50;
      lr.w = 80;
      lr.h = 80;
      if(SDL_IntersectRect(&lr,
            r,
            &result)) {
        nearby.emplace_back(l);
      }
    }
    UNLOCK_MUTEX(loot_list_mutex);
    return nearby;
  }
  bool Loot::is_gun() const {
    return object_type == type_t::GUN;
  }
  Loot::~Loot() {
    unregister_actor(&self);
  }
  void pickup_loot(const Loot* loot_ptr){
    LOCK_MUTEX(loot_list_mutex);
    if(loot_ptr->is_gun()){
      plr::get()->backpack->put_item(loot_ptr->id,type_t::GUN);
      m_debug("pickup_loot put_item gun");
    }else{
      plr::get()->backpack->put_item(loot_ptr->id,type_t::EXPLOSIVE);
      m_debug("pickup_loot put_item frag");
    }
    loot.remove_if([&](const auto& p){ return p->id == loot_ptr->id;});
    std::erase_if(loot_list,[&](const auto& p){ return p->id == loot_ptr->id;});
    UNLOCK_MUTEX(loot_list_mutex);
  }
};
