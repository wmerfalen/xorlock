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
    for(const auto& l : loot_list){
      r.x = l->where.x;
      r.y = l->where.y;
      draw::blatant_rect(&r);
    }
  }
  void program_exit(){
    flush_id();
  }
  void move_map(int dir, int amount){
    LOCK_MUTEX(loot_list_mutex);
    for(auto& l: loot_list){
      switch(dir) {
        case NORTH_EAST:
          l->where.y += amount;
          l->where.x -= amount;
          break;
        case NORTH_WEST:
          l->where.y += amount;
          l->where.x += amount;
          break;
        case NORTH:
          l->where.y += amount;
          break;
        case SOUTH_EAST:
          l->where.y -= amount;
          l->where.x -= amount;
          break;
        case SOUTH_WEST:
          l->where.y -= amount;
          l->where.x += amount;
          break;
        case SOUTH:
          l->where.y -= amount;
          break;
        case WEST:
          l->where.x += amount;
          break;
        case EAST:
          l->where.x -= amount;
          break;
        default:
          break;
      }
    }
    UNLOCK_MUTEX(loot_list_mutex);
  }
  void load_tiers(){
    // TODO: load tiered randomized weapon data
  }
  Loot::Loot(int npc_type,int npc_id,int in_cx,int in_cy){
    id = loot_next_id();
    loot_id = id;
    where.x = in_cx;
    where.y = in_cy;
    if(npc_type == constants::npc_type_t::NPC_SPETSNAZ){
      m_debug("SPETSNAZ. dropping pistol only");
      object_type = type_t::GUN;
      name = "Glock";
      // Only drop pistols
      type = type_t::GUN;
      item_type = wpn::weapon_type_t::WPN_T_PISTOL;
      decltype(weapons::pistol::data::p226::stats) drop_stats;
      //drop_stats[WPN_FLAGS] = 0;
      //drop_stats[WPN_TYPE] = 0;
      drop_stats[WPN_DMG_LO] = rand_between(15,95);
      drop_stats[WPN_DMG_HI] = rand_between(100,155);
      drop_stats[WPN_BURST_DLY] = 3;
      drop_stats[WPN_PIXELS_PT] = 38;
      drop_stats[WPN_CLIP_SZ] = rand_between(25,55);
      drop_stats[WPN_AMMO_MX] = drop_stats[WPN_CLIP_SZ] * rand_between(4,10);
      drop_stats[WPN_RELOAD_TM] = rand_between(1000,5000);
      drop_stats[WPN_COOLDOWN_BETWEEN_SHOTS] = rand_between(120,550);
      //drop_stats[WPN_MS_REGISTRATION] = 0;
      drop_stats[WPN_MAG_EJECT_TICKS] = rand_between(250,800);
      drop_stats[WPN_PULL_REPLACEMENT_MAG_TICKS] = rand_between(250,550);
      drop_stats[WPN_LOADING_MAG_TICKS] = rand_between(250,800);
      drop_stats[WPN_SLIDE_PULL_TICKS] = rand_between(250,550);
      drop_stats[WPN_WIELD_TICKS] = rand_between(200,800);
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
    }
    write_to_file();
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
    m_debug("id: " << id << "\nwhere.x: " << where.x << "\nwhere.y: " << where.y);
  }
  void dispatch(constants::npc_type_t npc_type, npc_id_t id, int in_cx, int in_cy){
    m_debug("npc died");
    auto ptr = std::make_unique<Loot>((int)npc_type,//int npc_type,
        (int)id, //int npc_id,
        (int)in_cx,//int cx, 
        (int)in_cy);
    loot_list.emplace_back(ptr.get());
    loot.emplace_front(std::move(ptr));
  }
  std::vector<Loot*> near_loot(SDL_Rect* r){
    std::vector<Loot*> nearby;
    SDL_Rect result;
    for(const auto& l : loot_list){
      SDL_Rect lr;
      lr.x = l->where.x - 50;
      lr.y = l->where.y - 50;
      lr.w = 80;
      lr.h = 80;
      if(SDL_IntersectRect(&lr,
            r,
            &result)) {
        nearby.emplace_back(l);
      }
    }
    return nearby;
  }
  void pickup_loot(const Loot* loot_ptr){
    LOCK_MUTEX(loot_list_mutex);
    switch(loot_ptr->item_type){
      case wpn::weapon_type_t::WPN_T_PISTOL:
        plr::get()->backpack->put_item(loot_ptr->id,type_t::GUN);
        m_debug("pickup_loot fed the pistol object");
        break;
      default:
        break;
    }
    loot.remove_if([&](const auto& p){ return p->id == loot_ptr->id;});
    std::erase_if(loot_list,[&](const auto& p){ return p->id == loot_ptr->id;});
    UNLOCK_MUTEX(loot_list_mutex);
  }
};
