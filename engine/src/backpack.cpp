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
  template <typename TStorageContainer>
  static inline void load_folder(const std::string& dir_name, TStorageContainer* storage ){
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
      if(s.compare(".") == 0 || s.compare("..") == 0 || s.compare("loot-id") == 0){
        continue;
      }
      std::string tmp = dir_name;
      if(tmp[tmp.length()-1] != '/'){
        tmp += "/";
      }
      tmp += s;
      storage->emplace_back(s);
    }
    closedir(fp);
  }
  void init(){
    m_debug("init");
  }

  void tick(){
  }
  void program_exit(){
  }
  Backpack::Backpack(){
  }
  void Backpack::load(){
    std::vector<std::string> files;
    load_folder(constants::loot_dir,&files);
    for(const auto& file : files){
      m_debug("file: '" << file << "'");
    }
  }
};
