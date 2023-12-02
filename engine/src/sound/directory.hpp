#include "gunshot.hpp"
#include <pthread.h>
#include <mutex>
#include "../tick.hpp"
#include <SDL2/SDL_mixer.h>
#include <sys/types.h>
#include <dirent.h>

#define m_debug(A) std::cout << "[SOUND][GUNSHOT][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[SOUND][GUNSHOT][ERROR]: " << A << "\n";
namespace sound::dir {
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
      if(s.find(".wav") == std::string::npos){
        continue;
      }
      std::string tmp = dir_name;
      if(tmp[tmp.length()-1] != '/'){
        tmp += "/";
      }
      tmp += s;
      auto created = Mix_LoadWAV(tmp.c_str());
      if(created == nullptr){
        m_error("couldnt open: '" << tmp << "'");
        continue;
      }
      m_debug("loading wav: '" << tmp << "'");
      storage->emplace_back(s,created);
    }
  }
  template <typename TStorageContainer>
  static inline void load_folder_list(const std::string& dir_name, TStorageContainer* storage ){
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
      if(s.find(".wav") == std::string::npos){
        continue;
      }
      std::string tmp = dir_name;
      if(tmp[tmp.length()-1] != '/'){
        tmp += "/";
      }
      tmp += s;
      auto created = Mix_LoadWAV(tmp.c_str());
      if(created == nullptr){
        m_error("couldnt open: '" << tmp << "'");
        continue;
      }
      m_debug("loading wav: '" << tmp << "'");
      storage->emplace_back(created);
    }
  }
};
