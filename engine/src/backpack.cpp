#include "backpack.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "filesystem.hpp"
#include "weapons/pistol/p226.hpp"

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
    FILE* fp = fopen(constants::backpack_file,"w");
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
      if(s.compare(".") == 0 || s.compare("..") == 0 || s.compare("loot-id") == 0 || s.compare("backpack") == 0){
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
    std::vector<std::pair<uint64_t,std::string>> v;
    for(int i=0; i < 16;i++){
      v.emplace_back(i * 10,std::to_string(i * 10));
    }
    write_backpack_id_list(&v);
  }

  void tick(){
  }
  void program_exit(){
  }
  Backpack::Backpack(){
  }
  void Backpack::load(){
    std::vector<std::string> files;
    auto v = load_backpack_id_list();
    load_folder(constants::loot_dir,&files,v);
    for(const auto& file : files){
      m_debug("file: '" << file << "'");
    }
  }
};
