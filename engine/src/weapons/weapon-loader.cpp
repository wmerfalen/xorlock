#include "weapon-loader.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <forward_list>
#include <functional>
#include <string.h>
#include "../rng.hpp"
#include "../db.hpp"

namespace wpn {
  void status(std::string m){
    std::cout << "[status]: " << m;
  }
  void done(){
    std::cout << "... DONE\n";
  }
  std::vector<std::string> weapon_file_names(){
    std::vector<std::string> list;
    for(const auto& type: {
        "ar",
        "smg",
        "shotgun",
        "sniper",
        "dmr",
        "pistol",
        "mp",
        }){
      std::string file = "../assets/vault-";
      file += type;
      file += ".xordb";
      list.emplace_back(file);
    }
    return list;
  }
  template <typename T>
    std::vector<std::string> file_names(){
      return {};
    }
  template <>
    std::vector<std::string> file_names<weapon_data_t>(){
      return weapon_file_names();
    }
  template <>
    std::vector<std::string> file_names<magazine_data_t>(){
      std::string file = "../assets/vault-attachment-magazine.xordb";
      return {file};
    }
  template <>
    std::vector<std::string> file_names<underbarrel_data_t>(){
      std::string file = "../assets/vault-attachment-underbarrel.xordb";
      return {file};
    }
  std::vector<std::string> attachment_file_names(){
    std::vector<std::string> list;
    for(const auto& type: {
        "muzzle",
        "barrel",
        "underbarrel",
        "magazine",
        "scope",
        }){
      std::string file = "../assets/vault-attachment-";
      file += type;
      file += ".xordb";
      list.emplace_back(file);
    }
    return list;
  }
  void truncate_all_files(){
    for(const auto& file : weapon_file_names()){
      FILE* fp = ::fopen(file.c_str(),"w+");
      if(fp){
        fclose(fp);
      }
    }
    for(const auto& file : attachment_file_names()){
      FILE* fp = ::fopen(file.c_str(),"w+");
      if(fp){
        fclose(fp);
      }
    }
  }
  template <typename TData>
    std::forward_list<std::unique_ptr<weapon_loader_t<TData>>> create_package(){
      std::forward_list<std::unique_ptr<weapon_loader_t<TData>>> list;
      for(const auto& file: file_names<TData>() ){
        FILE* fp = ::fopen(file.c_str(),"r");
        if(fp){
          list.emplace_front(std::make_unique<weapon_loader_t<TData>>(fp,true));
          fclose(fp);
        }
      }
      return list;
    }
  namespace vault {
    void create_mag_record(uint16_t low_seed, uint16_t hi_seed,const char* name, const char* manu, const char* desc, magazine_data_t* input);
  };
  void create_mags(){
    uint16_t low_seed, hi_seed;
    magazine_data_t w;

    memset(&w,0,sizeof(w));
    low_seed = rng::between(5,40);
    hi_seed = rng::between(15,80);
    vault::create_mag_record(low_seed, hi_seed,"EX-450AR","P7T Corp",
        "Extended magazine", 
        &w);

    memset(&w,0,sizeof(w));
    low_seed = rng::between(7,50);
    hi_seed = rng::between(25,90);
    vault::create_mag_record(low_seed, hi_seed,"EX-50DR","P7T Corp",
        "Extended drum magazine", 
        &w);

    memset(&w,0,sizeof(w));
    low_seed = rng::between(rng::between(7,10),rng::between(50,60));
    hi_seed = rng::between(rng::between(25,35),rng::between(90,110));
    vault::create_mag_record(low_seed, hi_seed,"Hellfire VI","SECTOR SIX LLC",
        "![HCAP,HV,INC]High capacity, high velocity, incendiary ammunition.", 
        &w);
  }
  namespace vault {
    static std::forward_list<std::unique_ptr<weapon_loader_t<weapon_data_t>>> wpn_vault_contents;
    static std::forward_list<std::unique_ptr<weapon_loader_t<magazine_data_t>>> att_mag_vault_contents;
    static std::forward_list<std::unique_ptr<weapon_loader_t<underbarrel_data_t>>> att_ubar_vault_contents;
    void dump_database(){
      status("wpn vault");
      for(const auto & v : wpn_vault_contents){
        v->dump_contents();
      }
      done();
      status("mag vault");
      for(const auto & v : att_mag_vault_contents){
        v->dump_contents();
      }
      done();
      status("ubar vault");
      for(const auto & v : att_ubar_vault_contents){
        v->dump_contents();
      }
      done();
    }
    void create_weapons();
    void create_default_packages(){
      wpn_vault_contents = create_package<weapon_data_t>();
      att_mag_vault_contents = create_package<magazine_data_t>();
      att_ubar_vault_contents = create_package<underbarrel_data_t>();
      for(const auto & wpn_vault : wpn_vault_contents){
        wpn_vault->dump_contents();
      }
      for(const auto & att_mag_vault : att_mag_vault_contents){
        att_mag_vault->dump_contents();
      }
      for(const auto & att_ubar_vault : att_ubar_vault_contents){
        att_ubar_vault->dump_contents();
      }
    }
    void init(int argc, char** argv){
      bool should_create_weapons = false;
      bool should_create_mags = false;
      bool should_init = false;
      std::size_t how_many = 0;
      for(unsigned i=1; i < argc;++i){
        if(std::string(argv[i]).compare("--create-weapons") == 0){
          should_create_weapons = true;
          continue;
        }
        if(std::string(argv[i]).compare("--create-mags") == 0){
          should_create_mags = true;
          continue;
        }
        if(std::string(argv[i]).substr(0,strlen("--count=")).compare("--count=") == 0){
          std::string a = argv[i];
          a = a.substr(strlen("--count="));
          how_many = atoi(a.data());
          continue;
        }
        if(std::string(argv[i]).substr(0,strlen("--init")).compare("--init") == 0){
          should_init = true;
          continue;
        }
      }
      if(should_init){
        status("truncating all weapon vaults...");
        truncate_all_files();
        done();
      }
      if(should_create_mags){
        if(how_many){
          status(std::string("building ") + std::to_string(how_many) + " mags");
          for(auto i=0; i < how_many;i++){
            create_mags();
          }
          done();
        }else{
          status("Creating default amount of mags");
          create_mags();
          done();
        }
      }
      if(should_create_weapons){
        if(how_many){
          status(std::string("building ") + std::to_string(how_many) + " weapons");
          for(auto i=0; i < how_many;i++){
            create_weapons();
          }
          done();
        }else{
          status("Creating default amount of weapons");
          create_weapons();
          done();
        }
      }
      create_default_packages();
      dump_database();
    }
    template <typename TData>
      void write_names(TData* w,const std::string& name,
          const std::string& manu,
          const std::string& desc){

        memset(&w->name[0],0,sizeof(w->name));
        memset(&w->manufacturer[0],0,sizeof(w->manufacturer));
        memset(&w->description[0],0,sizeof(w->description));
        strncpy(&w->name[0],name.data(),std::min(name.length(),sizeof(w->name)));
        strncpy(&w->manufacturer[0],manu.data(),std::min(manu.length(),sizeof(w->manufacturer)));
        strncpy(&w->description[0],desc.data(),std::min(desc.length(),sizeof(w->description)));
      }
    template <typename TResult>
      auto lo_dmg_buff = [](TResult& lo, TResult& hi) -> void {
        lo += lo * 0.5;
        hi += hi * 0.8;
      };
    template <typename TResult>
      auto hi_dmg = [](TResult& lo, TResult& hi) -> void {
        lo += lo * 1.5;
        lo += lo;
        hi += hi * 4.0;
      };
    template <typename TResult>
      auto massive_dmg = [](TResult& lo, TResult& hi) -> void {
        lo += lo * 10.5;
        lo += lo;
        hi += hi * 40.0;

      };
    template <typename TResult>
      std::pair<TResult,TResult> rng_seed(TResult lo, TResult hi){
        TResult our_lo = lo, our_hi = hi;
        if(rng::chaos()){
          std::cout << "[BONUS]: lo_dmg_buff\n";
          lo_dmg_buff<TResult>(our_lo,our_hi);
        }
        if(rng::chaos()){
          std::cout << "[BONUS]: hi_dmg_buff\n";
          hi_dmg<TResult>(our_lo,our_hi);
        }
        if(rng::chaos() && rng::chaos() == false && rng::chaos()){
          std::cout << "[BONUS]: hi_dmg_buff (TIMES 3!!!)\n";
          hi_dmg<TResult>(our_lo,our_hi);
          hi_dmg<TResult>(our_lo,our_hi);
          hi_dmg<TResult>(our_lo,our_hi);
        }
        if(rng::chaos() && rng::chaos() == false && rng::chaos() == false && rng::chaos()){
          std::cout << "[BONUS]: massive_dmg_buff! HOLY SHIT!\n";
          massive_dmg<TResult>(our_lo,our_hi);
        }
        if(rng::chaos() && rng::chaos() == false && rng::chaos() == false && rng::chaos()){
          if(rng::chaos() && rng::chaos() == false && rng::chaos() == false && rng::chaos()){
            if(rng::chaos() && rng::chaos() == false && rng::chaos() == false && rng::chaos()){
              std::cout << "[INSANE BONUS]: massive_dmg_buff (TIME 3!!!) HOLY ** FUCKING ** SHIT!\n";
              massive_dmg<TResult>(our_lo,our_hi);
              massive_dmg<TResult>(our_lo,our_hi);
              massive_dmg<TResult>(our_lo,our_hi);
            }
          }
        }
        return {our_lo,our_hi};
      }
    template <typename TResult>
      TResult squirrelly_rng(TResult lo){
        TResult our_lo = lo,_junk;
        if(rng::chaos()){
          std::cout << "[BONUS]: lo_dmg_buff\n";
          lo_dmg_buff<TResult>(our_lo,_junk);
          if(rng::chaos()){
            std::cout << "[EXTRA BONUS]: adding an additional " << _junk << " to your baseline of: " << our_lo << "\n";
            our_lo += _junk;
          }else if(rng::chaos() && !rng::chaos()){
            std::cout << "[EXTRA BONUS]: adding an additional " << _junk * 2.0 << " to your baseline of: " << our_lo << "\n";
            our_lo += _junk * 2.0;
          }else if(rng::between(1,1000) > 990){
            std::cout << "[EXTRA BONUS]: tripling your baseline of " << our_lo << "to: " << our_lo * 3 << "\n";
            our_lo *= 3;
          }
        }
        _junk = rng::between(1,10);
        if(rng::between(1,50) > 40){
          _junk *= 2;
        }
        if(rng::chaos() && rng::between(1,10) > 9){
          std::cout << "[BONUS]: hi_dmg_buff\n";
          hi_dmg<TResult>(our_lo,_junk);
        }
        if(rng::chaos() && rng::chaos() == false && rng::chaos()){
          std::cout << "[BONUS]: hi_dmg_buff (TIMES 3!!!)\n";
          hi_dmg<TResult>(our_lo,++_junk);
          hi_dmg<TResult>(our_lo,++_junk);
          hi_dmg<TResult>(our_lo,++_junk);
        }
        if(rng::chaos() && rng::chaos() == false && rng::chaos() == false && rng::chaos()){
          std::cout << "[BONUS]: massive_dmg_buff! HOLY SHIT!\n";
          massive_dmg<TResult>(our_lo,_junk * rng::between(1,4));
        }
        if(rng::chaos() && rng::chaos() == false && rng::chaos() == false && rng::chaos()){
          if(rng::chaos() && rng::chaos() == false && rng::chaos() == false && rng::chaos()){
            if(rng::chaos() && rng::chaos() == false && rng::chaos() == false && rng::chaos()){
              std::cout << "[INSANE BONUS]: massive_dmg_buff (TIME 3!!!) HOLY ** FUCKING ** SHIT!\n";
              massive_dmg<TResult>(our_lo,_junk * rng::between(1,4));
              massive_dmg<TResult>(our_lo,_junk * rng::between(1,4));
              massive_dmg<TResult>(our_lo,_junk * rng::between(1,4));
            }
          }
        }
        return our_lo;
      }
    template <typename TResult>
      TResult chaotic_low_value_generator(TResult base,TResult threshold){
        if(rng::chaos()){
          auto val = rng::between(base / 2,base);
          std::cout << "[NERF]: base: " << base << " becomes lower: " << val << "\n";
          base -= rng::between(base / 2,base);
        }
        if(rng::between(1,10) > rng::between(1,10) || rng::between(1,5) < rng::between(1,5)){
          if(rng::chaos()){
            if(rng::chaos() == false && rng::chaos() == false && rng::chaos() == true){
              std::cout << "[NERF]: ULTRA LUCKY: Massive decrements for you, my dude.\n";
              base -= rng::between(4,8);
            }
          }
        }
        return base;
      }

    using mag_type_t = magazine_data_t::magazine_type_t;
    mag_type_t random_mag_type(){
      std::array<mag_type_t,5> types = {
        mag_type_t::MAG_T_INCENDIARY,
        mag_type_t::MAG_T_HIGH_VELOCITY,
        mag_type_t::MAG_T_TRACER,
        mag_type_t::MAG_T_EXPLOSIVE,
        mag_type_t::MAG_T_HIGH_CAPACITY,
      };
      uint8_t set=0;
      uint16_t value = 0;
      bool chaos = rng::chaos();
      chaos = rng::chaos();
      if(!chaos || rng::between(1,10) > 8) {
        value |= types[0];
        ++set;
      }
      chaos = rng::chaos();
      if(chaos && rng::between(1,20) > 18) {
        value |= types[1];
        ++set;
      }
      chaos = rng::chaos();
      if(chaos && rng::between(1,50) > 45) {
        value |= types[2];
        ++set;
      }
      chaos = rng::chaos();
      if(!chaos && rng::between(1,80) > 75) {
        value |= types[3];
        ++set;
      }
      chaos = rng::chaos();
      if(chaos && rng::between(1,20) > 18) {
        value |= types[4];
        ++set;
      }
      if(!set){
        return random_mag_type();
      }
      return (mag_type_t)value;
    }
    const auto check_ar = []() -> std::pair<bool,weapon_type_t> {
      return {rng::between(1,10) > 8,weapon_type_t::WPN_T_AR};
    };
    const auto check_smg = []() -> std::pair<bool,weapon_type_t> {
      return {rng::between(1,20) > 15,weapon_type_t::WPN_T_SMG};
    };
    const auto check_shotgun = []() -> std::pair<bool,weapon_type_t> {
      return {rng::between(1,30) > 25,WPN_T_SHOTGUN};
    };
    const auto check_sniper = []() -> std::pair<bool,weapon_type_t> {
      return {rng::between(1,80) > 75,WPN_T_SNIPER};
    };
    const auto check_lmg = []() -> std::pair<bool,weapon_type_t> {
      return {rng::between(1,80) > 75,WPN_T_LMG};
    };
    const auto check_dmr = []() -> std::pair<bool,weapon_type_t> {
      return {rng::between(1,70) > 62,WPN_T_DMR};
    };
    const auto check_pistol = []() -> std::pair<bool,weapon_type_t> {
      return {rng::between(1,20) > 10,WPN_T_PISTOL};
    };
    const auto check_machine_pistol = []() -> std::pair<bool,weapon_type_t> {
      return {rng::between(1,20) > 10,WPN_T_MACHINE_PISTOL};
    };
    weapon_type_t random_weapon_type(){
      std::vector<std::function<std::pair<bool,weapon_type_t>()>> logic;
      for(const auto& cb : std::vector<std::function<std::pair<bool,weapon_type_t>()>>{
          check_pistol,
          check_machine_pistol,
          check_ar,
          check_smg,
          check_shotgun,
          check_sniper,
          check_lmg,
          check_dmr}){
        logic.emplace_back(cb);
      }
      logic = rng::shuffle_container(logic);
      for(const auto& cb : logic){
        auto pair = cb();
        if(pair.first){
          return pair.second;
        }
      }
      return random_weapon_type();
    }
    bool has_force_mag_type(magazine_data_t* w){
      std::string n = w->name;
      return n.substr(0,strlen("![")).compare("![") == 0;
    }
    mag_type_t extract_force_mag_type(magazine_data_t* w){
      using mag_t = magazine_data_t::magazine_type_t;
      std::string n = w->name;
      std::string current;
      uint16_t mag_type = 0;
      for(const auto& ch : n){
        if(ch == '!'){
          continue;
        }
        if(isalpha(ch)){
          current += ch;
          continue;
        }
        if(ch == ','){
          if(current.compare("HCAP") == 0){
            mag_type |= mag_t::MAG_T_HIGH_CAPACITY;
          }
          if(current.compare("INC") == 0){
            mag_type |=mag_t::MAG_T_INCENDIARY;
          }
          if(current.compare("HV") == 0){
            mag_type |=mag_t::MAG_T_HIGH_VELOCITY;
          }
          if(current.compare("TRC") == 0){
            mag_type |= mag_t::MAG_T_TRACER;
          }
          if(current.compare("EXP") == 0){
            mag_type |= mag_t::MAG_T_EXPLOSIVE;
          }
          current.clear();
          continue;
        }
        if(ch == '['){
          continue;
        }
        if(ch == ']'){
          break;
        }
      }
      if(current.length()){
        if(current.compare("HCAP") == 0){
          mag_type |= mag_t::MAG_T_HIGH_CAPACITY;
        }
        if(current.compare("INC") == 0){
          mag_type |=mag_t::MAG_T_INCENDIARY;
        }
        if(current.compare("HV") == 0){
          mag_type |=mag_t::MAG_T_HIGH_VELOCITY;
        }
        if(current.compare("TRC") == 0){
          mag_type |= mag_t::MAG_T_TRACER;
        }
        if(current.compare("EXP") == 0){
          mag_type |= mag_t::MAG_T_EXPLOSIVE;
        }
      }
      return (mag_t)mag_type;
    }

    void create_mag_record(uint16_t low_seed, uint16_t hi_seed,const char* name, const char* manu, const char* desc, magazine_data_t* input){
      magazine_data_t w;
      memset(&w,0,sizeof(w));
      write_names(&w,
          name,
          manu,
          desc
          );
      auto pair = rng_seed<uint16_t>(low_seed,hi_seed);
      w.id = db::next_id();
      w.created_at = time(nullptr);

      if(has_force_mag_type(&w)){
        w.type = extract_force_mag_type(&w);
      }else{
        w.type = random_mag_type();
      }
      w.attaches_to = random_weapon_type();
      if(w.type & mag_type_t::MAG_T_INCENDIARY) {
        w.incendiary_dice_sides = 2 + rng::between(1,5);
        w.incendiary_dice_count = 6 + rng::between(4,14);
      }else{
        w.incendiary_dice_sides = 0;
        w.incendiary_dice_count = 0;
      }
      if(w.type & mag_type_t::MAG_T_HIGH_CAPACITY) {
        w.high_capacity_dice_sides = 4 + rng::between(3,10);
        w.high_capacity_dice_count = 2 + rng::between(4,14);
        w.clip_size = rng::roll(w.high_capacity_dice_count,w.high_capacity_dice_sides);
      }else{
        w.high_capacity_dice_sides = 0;
        w.high_capacity_dice_count = 0;
      }
      if(w.type & mag_type_t::MAG_T_HIGH_VELOCITY) {
        w.hv_dice_sides = 4 + rng::between(3,20);
        w.hv_dice_count = 2 + rng::between(4,14);
      }else{
        w.hv_dice_sides = 0;
        w.hv_dice_count = 0;
      }
      if(w.type & mag_type_t::MAG_T_TRACER){
        w.tracer_dice_sides = 4 + rng::between(3,20);
        w.tracer_dice_count = 2 + rng::between(4,14);
      }else{
        w.tracer_dice_sides = 0;
        w.tracer_dice_count = 0;
      }
      if(w.type & mag_type_t::MAG_T_EXPLOSIVE) {
        w.explosive_dice_sides = 4 + rng::between(3,20);
        w.explosive_dice_count = 2 + rng::between(4,14);
      }else{
        w.explosive_dice_sides = 0;
        w.explosive_dice_count = 0;
      }
      w.clip_size = 30 + (0.5 * rng::between(20,40));
      w.damage_dice_sides = pair.first;
      w.damage_dice_count = pair.second;
      w.modify_eject_ticks = - 10 - rng::between(1,8);
      w.modify_reload_ticks = - 30 - rng::between(1,20);
      w.modify_chamber_ticks = - 4 - rng::between(1,3);
      FILE* fp = fopen("../assets/vault-attachment-magazine.xordb","a");
      if(fp){
        std::cout << "saving magazine attachment: '" << w.name << "'\n";
        fwrite(&w,sizeof(magazine_data_t),1,fp);
        fclose(fp);
      }
    }
    void create_ar_record(uint16_t low_seed, uint16_t hi_seed,const char* name, const char* manu, const char* desc, weapon_data_t* input){
      weapon_data_t w;
      memset(&w,0,sizeof(w));
      write_names(&w,
          name,
          manu,
          desc
          );
      auto pair = rng_seed<uint16_t>(low_seed,hi_seed);
      w.id = db::next_id();
      w.created_at = time(nullptr);
      w.damage_dice_sides = pair.first;
      w.damage_dice_count = pair.second;
      w.pixels_per_tick = rng::between(20,50);
      w.clip_size = 30;
      w.ammo_max = 300 + rng::between(10,25);
      w.eject_ticks = 10 - rng::between(1,8);
      w.reload_ticks = 30 - rng::between(1,20);
      w.chamber_ticks = 4 - rng::between(1,3);
      w.ads_ticks = 10 - rng::between(1,4);
      w.cooldown_between_shots_ticks = 50 - rng::between(1,40);
      w.ammo_type = 1;
      w.damage_decay = 0.50 - (0.010 * rng::between(1,100));
      w.effective_range = 25 + rng::between(1,20);
      w.incendiary_base = 0;
      w.incendiary_dice_sides = 0;
      w.incendiary_dice_count = 0;
      w.incendiary_chance_dice_sides = 0;
      w.incendiary_chance_dice_count = 0;
      w.attached_scope_id = 0;
      w.attached_underbarrel_id = 0;
      w.attached_magazine_id = 0;
      w.attached_barrel_id = 0;
      w.type = WPN_T_AR;
      FILE* fp = fopen("../assets/vault-ar.xw","a");
      if(fp){
        std::cout << "saving weapon: '" << w.name << "'\n";
        fwrite(&w,sizeof(weapon_data_t),1,fp);
        fclose(fp);
      }
    }
    void create_weapons(){
      uint16_t low_seed, hi_seed;
      weapon_data_t w;

      memset(&w,0,sizeof(w));
      low_seed = rng::between(5,40);
      hi_seed = rng::between(15,80);
      create_ar_record(low_seed, hi_seed,"G36C","Heckler & Koch",
          "The Heckler & Koch G36 (Gewehr 36) is a 5.56×45mm NATO assault rifle designed in the early 1990s by German weapons manufacturer Heckler & Koch as a replacement for the heavier 7.62×51mm G3 battle rifle.", 
          &w);

      memset(&w,0,sizeof(w));
      low_seed = rng::between(8,60);
      hi_seed = rng::between(25,110);
      create_ar_record(low_seed, hi_seed,"TAR-21","IWI",
          "The IWI Tavor, is an Israeli bullpup assault rifle chambered in 5.56×45mm NATO calibre, designed and produced by Israel Weapon Industries (IWI). It is part of the Tavor family of rifles, which have spawned many derivatives of the original design.", &w);

      memset(&w,0,sizeof(w));
      low_seed = rng::between(18,80);
      hi_seed = rng::between(45,150);
      create_ar_record(low_seed, hi_seed,"F2000","FN Herstal",
          "The F2000 is a modular weapon system; its principal component is a compact 5.56×45mm NATO-caliber assault rifle in a bullpup configuration. The F2000 is a selective fire weapon operating from a closed bolt.",
          &w);

      memset(&w,0,sizeof(w));
      low_seed = rng::between(20,50);
      hi_seed = rng::between(45,50);
      create_ar_record(low_seed, hi_seed,"LR-300","Z-M Weapons",
          "The Z-M LR-300 is an American rifle. The model name stands for Light Rifle and 300 is for 300 meters, which is regarded by the manufacturer as the effective range of the rifle. The design is based on the AR-15, M16 and C7 rifles, but has a unique semi-direct gas impingement system and a folding stock option.",
          &w);
    }
  };
};
