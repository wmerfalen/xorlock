#ifndef __DB_HEADER__
#define __DB_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <string_view>
#include <memory>
#include "actor.hpp"
#include <map>
#include <memory>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace db {
  static uint64_t id = 0;
  void init(){
    struct stat statbuf;
    memset(&statbuf,0,sizeof(statbuf));
    int s =  stat("../assets/pkid", &statbuf);
    FILE* fp = nullptr;
    if(s != 0){
      fp = fopen("../assets/pkid","w+");
      if(fp){
        fwrite("1",1,1,fp);
        fclose(fp);
      }
    }
    fp = fopen("../assets/pkid","r+");
    char buffer[32];
    memset(buffer,0,sizeof(buffer));
    s = fread(&buffer[0],sizeof(char),sizeof(buffer)-1,fp);
    if(s){
      id = atoi(&buffer[0]);
    }else{
      id = 1;
    }
    if(fp){
      fclose(fp);
    }
    std::cout << "id: '" << id << "'\n";
  }
  uint64_t next_id(){
    std::string str_id;
    ++id;
    str_id = std::to_string(id);
    FILE* fp = fopen("../assets/pkid","w+");
    if(fp){
      fwrite(str_id.data(),sizeof(char),str_id.length(),fp);
      fclose(fp);
    }else{
      std::cerr << "ERROR: COULDNT WRITE PKID TO FILE\n";
    }
    return id;
  }
};

#endif
