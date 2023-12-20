#include "backpack.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "filesystem.hpp"
#include "weapons/pistol/p226.hpp"
#include "player.hpp"

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[BACKPACK][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[BACKPACK][ERROR]: " << A << "\n";

namespace backpack {
  static inline std::vector<std::pair<uint64_t,std::string>> load_backpack_id_list(){
    std::vector<std::pair<uint64_t,std::string>> v;
    FILE* fp = fopen(constants::backpack_file,"r");
    if(fp == nullptr){
      return v;
    }
    std::string msg;
    while(!feof(fp)){
      char tmp = fgetc(fp);
      if(tmp == '\n'){
        if(msg.length()){
          v.emplace_back(atol(msg.c_str()),msg);
          msg.clear();
        }
      }else{
        msg += tmp;
      }
    }
    fclose(fp);
    return v;
  }
  static inline void write_backpack_id_list(std::vector<std::pair<uint64_t,std::string>>* id_list){
    FILE* fp = fopen(constants::backpack_file,"w+");
    if(fp == nullptr){
      m_error("write_backpack_id_list failed to open backpack_file: " << strerror(errno));
      return;
    }
    std::string s;
    for(const auto& id : *id_list){
      s = id.second;
      s += "\n";
      fwrite(s.c_str(),sizeof(char),s.length(),fp);
    }
    fclose(fp);
  }
  template <typename TStorageContainer>
  static inline void load_folder(const std::string& dir_name, TStorageContainer* storage ,const std::vector<std::pair<uint64_t,std::string>>& filter){
    m_debug("load_folder: " << dir_name);
    DIR * fp = opendir(dir_name.c_str());
    if(!fp){
      m_error("UNABLE to open '" << dir_name << "' directory");
      return;
    }
    struct dirent * dp = nullptr;
    while((dp = readdir(fp)) != nullptr){
      std::string s = dp->d_name;
      m_debug("checking: '" << s << "'");
      if(s.find_first_not_of("0123456789") != std::string::npos){
        continue;
      }
      std::string tmp = dir_name;
      if(tmp[tmp.length()-1] != '/'){
        tmp += "/";
      }
      tmp += s;
      for(const auto& id : filter){
        if(s.compare(id.second.c_str()) == 0){
          storage->emplace_back(s);
        }
      }
    }
    closedir(fp);
  }
  void init(){
    m_debug("init");
  }

  void tick(){
  }
  void program_exit(){
    std::string dir_name = constants::loot_dir;
    auto v = load_backpack_id_list();
    m_debug("load_folder: " << dir_name);
    DIR * fp = opendir(dir_name.c_str());
    if(!fp){
      m_error("UNABLE to open '" << dir_name << "' directory");
      return;
    }
    struct dirent * dp = nullptr;
    bool remove;
    while((dp = readdir(fp)) != nullptr){
      std::string s = dp->d_name;
      if(s.find_first_not_of("0123456789") != std::string::npos){
        continue;
      }
      remove = true;
      for(const auto& p : v){
        if(p.second.compare(s.c_str()) == 0){
          remove = false;
          break;
        }
      }
      if(remove){
        std::string p = dir_name;
        p += s;
        unlink(p.c_str());
      }
    }
    closedir(fp);
  }
  Backpack::Backpack(){
  }
  type_t get_type_from_file(std::string file){
    ExportWeapon hdr;
    FILE* fp = fopen(file.c_str(),"r");
    fread(&hdr,sizeof(type_t),1,fp);
    fclose(fp);
    return hdr.object_type;
  }
  void Backpack::load(){
    std::vector<std::string> files;
    auto v = load_backpack_id_list();
    load_folder(constants::loot_dir,&files,v);
    for(const auto& file : files){
      m_debug("file: '" << file << "'");
      loot_id_t id = atol(file.c_str());
      auto t = put_item(id,get_type_from_file(std::string(constants::loot_dir) + file));
      if(!std::get<0>(t)){
        m_error("Unable to load loot file: " << file << ": '" << std::get<1>(t) << "'");
      }
    }
  }
  std::tuple<bool,std::string> Backpack::put_item(const loot_id_t & id,type_t type){
    std::string dir = constants::loot_dir;
    dir += std::to_string(id);
    m_debug("dir: '" << dir << "'");
    FILE* fp = fopen(dir.c_str(),"r");
    if(!fp){
      return {false,"Couldn't open file"};
    }
    bool do_save = false;

    if(type == type_t::GUN){
      auto ptr = std::make_unique<ExportWeapon>();
      fread(ptr.get(),sizeof(ExportWeapon),1,fp);
      weapons.emplace_front(std::move(ptr));
      do_save = true;
    }
    if(type == type_t::EXPLOSIVE){
      auto ptr = std::make_unique<ExportGrenade>();
      fread(ptr.get(),sizeof(ExportGrenade),1,fp);
      grenades.emplace_front(std::move(ptr));
      do_save = true;
    }
    fclose(fp);
    if(do_save){
      refresh();
      save();
    }
    return {true,""};
  }
  void Backpack::remove_item(const loot_id_t& id){
    weapons.remove_if([&](auto& ptr) -> bool {
        return ptr->id == id;
        });
    grenades.remove_if([&](auto& ptr) -> bool {
        return ptr->id == id;
        });
    refresh();
    save();
  }
  
  void Backpack::refresh(){
    weapons_ptr.clear();
    grenades_ptr.clear();
    for(const auto& p : weapons){
      weapons_ptr.emplace_back(p.get());
    }
    for(const auto& p : grenades){
      grenades_ptr.emplace_back(p.get());
    }
  }
  void Backpack::save(){
    std::vector<std::pair<uint64_t,std::string>> id_list;
    for(const auto& w : weapons){
      id_list.emplace_back(w->id,std::to_string(w->id));
    }
    for(const auto& g : grenades){
      id_list.emplace_back(g->id,std::to_string(g->id));
    }
    write_backpack_id_list(&id_list);
  }
  std::pair<bool,std::string> Backpack::wield_primary(ExportWeapon* ptr){
    if(ptr == nullptr){
      m_error("invalid weapon");
      return {false,"Invalid weapon"};
    }
    std::string dir = constants::loot_dir;
    dir += "primary";
    m_debug("dir: '" << dir << "'");
    FILE* fp = fopen(dir.c_str(),"w+");
    if(!fp){
      return {false,"Couldn't open file"};
    }
    fwrite(std::to_string(ptr->id).c_str(),sizeof(char),std::to_string(ptr->id).length(),fp);
    fwrite("\n",sizeof(char),1,fp);
    fclose(fp);
    plr::get()->equip_weapon(1,&ptr->stats,nullptr);
    return {true,"Wielded primary"};
  }

  std::pair<bool,std::string> Backpack::wield_secondary(ExportWeapon* ptr){
    if(ptr == nullptr){
      m_error("invalid weapon (secondary)");
      return {false,"Invalid weapon"};
    }
    std::string dir = constants::loot_dir;
    dir += "secondary";
    m_debug("dir: '" << dir << "'");
    FILE* fp = fopen(dir.c_str(),"w+");
    if(!fp){
      return {false,"Couldn't open file"};
    }
    fwrite(std::to_string(ptr->id).c_str(),sizeof(char),std::to_string(ptr->id).length(),fp);
    fwrite("\n",sizeof(char),1,fp);
    fclose(fp);

    plr::get()->equip_weapon(0,&ptr->stats,nullptr);
    return {true,"Wielded secondary"};
  }

  std::pair<bool,std::string> Backpack::wield_frag(ExportGrenade* ptr){
    if(ptr == nullptr){
      return {false,"Invalid grenade"};
    }
    std::string dir = constants::loot_dir;
    dir += "explosive0";
    m_debug("dir: '" << dir << "'");
    FILE* fp = fopen(dir.c_str(),"w+");
    if(!fp){
      return {false,"Couldn't open file"};
    }
    fwrite(&ptr->id,sizeof(ptr->id),1,fp);
    fwrite("\n",sizeof(char),1,fp);
    fclose(fp);
    plr::get()->equip_weapon(2,nullptr,&ptr->stats);
    return {true,"Wielded grenade"};
  }
  ExportWeapon* Backpack::get_primary(){
    std::string dir = constants::loot_dir;
    dir += "primary";
    m_debug("dir: '" << dir << "'");
    FILE* fp = fopen(dir.c_str(),"r");
    if(!fp){
      m_debug("!fp");
      return nullptr;
    }
    static constexpr size_t BUF_SIZE = 16;
    std::array<char,BUF_SIZE> buf;
    std::fill(buf.begin(),buf.end(),0);
    size_t bytes = fread(&buf[0],sizeof(char),BUF_SIZE,fp);
    if(bytes <= 0){
      m_debug("couldn't read primary file");
      return nullptr;
    }
    std::string current;
    for(const auto& c : buf){
      if(isdigit(c)){
        current += c;
        continue;
      }
      if(c == '\n'){
        break;
      }
    }
    m_debug("current: '" << current << "'");
    uint64_t loot_id = atol(current.c_str());
    for(const auto& ptr : weapons_ptr){
      if(ptr->id == loot_id){
        m_debug("found primary by loot_id: " << loot_id);
        return ptr;
      }
    }
    m_debug("couldn't find primary by id:" << loot_id);
    return nullptr;
  }
  ExportWeapon* Backpack::get_secondary(){
    std::string dir = constants::loot_dir;
    dir += "secondary";
    m_debug("dir: '" << dir << "'");
    FILE* fp = fopen(dir.c_str(),"r");
    if(!fp){
      m_debug("!fp");
      return nullptr;
    }
    static constexpr size_t BUF_SIZE = 16;
    std::array<char,BUF_SIZE> buf;
    std::fill(buf.begin(),buf.end(),0);
    size_t bytes = fread(&buf[0],sizeof(char),BUF_SIZE,fp);
    if(bytes <= 0){
      m_debug("couldn't read secondary file");
      return nullptr;
    }
    std::string current;
    for(const auto& c : buf){
      if(isdigit(c)){
        current += c;
        continue;
      }
      if(c == '\n'){
        break;
      }
    }
    m_debug("current: '" << current << "'");
    uint64_t loot_id = atoi(current.c_str());
    for(const auto& ptr : weapons_ptr){
      if(ptr->id == loot_id){
        m_debug("found secondary by loot_id: " << loot_id);
        return ptr;
      }
    }
    m_debug("couldn't find secondary by id:" << loot_id);
    return nullptr;
  }
  ExportGrenade* Backpack::get_frag(){
    std::string dir = constants::loot_dir;
    dir += "explosive0";
    m_debug("dir: '" << dir << "'");
    FILE* fp = fopen(dir.c_str(),"r");
    if(!fp){
      m_debug("!fp");
      return nullptr;
    }
    static constexpr size_t BUF_SIZE = 16;
    std::array<char,BUF_SIZE> buf;
    std::fill(buf.begin(),buf.end(),0);
    size_t bytes = fread(&buf[0],sizeof(char),BUF_SIZE,fp);
    if(bytes <= 0){
      m_debug("couldn't read explosive0 file");
      return nullptr;
    }
    std::string current;
    for(const auto& c : buf){
      if(isdigit(c)){
        current += c;
        continue;
      }
      if(c == '\n'){
        break;
      }
    }
    m_debug("current: '" << current << "'");
    uint64_t loot_id = atoi(current.c_str());
    for(const auto& ptr : grenades_ptr){
      if(ptr->id == loot_id){
        m_debug("found grenade by loot_id: " << loot_id);
        return ptr;
      }
    }
    m_debug("couldn't find grenade by id:" << loot_id);
    return nullptr;
  }

};
