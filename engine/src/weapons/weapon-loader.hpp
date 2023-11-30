#ifndef __WEAPONS_WEAPON_LOADER_HEADER__
#define __WEAPONS_WEAPON_LOADER_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <memory>
#include <forward_list>

namespace wpn {
  enum weapon_type_t : uint16_t {
    WPN_T_AR = 0,
    WPN_T_SMG = 1,
    WPN_T_SHOTGUN = 2,
    WPN_T_PISTOL = 3,
    WPN_T_MACHINE_PISTOL = 4,
    WPN_T_SNIPER = 5,
    WPN_T_LMG = 6,
    WPN_T_DMR = 7,
  };
  static constexpr std::array<weapon_type_t,8> WEAPON_TYPES = {
        weapon_type_t::WPN_T_AR,
        weapon_type_t::WPN_T_SMG,
        weapon_type_t::WPN_T_SHOTGUN,
        weapon_type_t::WPN_T_PISTOL,
        weapon_type_t::WPN_T_MACHINE_PISTOL,
        weapon_type_t::WPN_T_SNIPER,
        weapon_type_t::WPN_T_LMG,
        weapon_type_t::WPN_T_DMR,
      };
  struct weapon_data_t {
    uint64_t id;
    uint64_t created_at;
    weapon_type_t type;
    char name[32];
    char manufacturer[32];
    char description[256];
    uint16_t damage_dice_sides;
    uint16_t damage_dice_count;
    uint16_t pixels_per_tick;
    uint16_t clip_size;
    uint16_t ammo_max;
    uint16_t eject_ticks;
    uint16_t reload_ticks;
    uint16_t chamber_ticks;
    uint16_t ads_ticks;
    uint16_t cooldown_between_shots_ticks;
    uint32_t ammo_type;
    uint16_t incendiary_base;
    uint16_t incendiary_dice_sides;
    uint16_t incendiary_dice_count;
    uint16_t incendiary_chance_dice_sides;
    uint16_t incendiary_chance_dice_count;
    uint64_t attached_scope_id;
    uint64_t attached_underbarrel_id;
    uint64_t attached_magazine_id;
    uint64_t attached_barrel_id;
    uint64_t attached_muzzle_id;
    float damage_decay;
    float effective_range;
  };
  struct magazine_data_t {
    enum magazine_type_t : uint16_t {
      MAG_T_INCENDIARY = (1 << 0),
      MAG_T_HIGH_VELOCITY = (1 << 1),
      MAG_T_TRACER = (1 << 2),
      MAG_T_EXPLOSIVE = (1 << 3),
      MAG_T_HIGH_CAPACITY = (1 << 4),
    };
    uint64_t id;
    uint64_t created_at;
    char name[32];
    char manufacturer[32];
    char description[256];
    uint16_t damage_dice_sides;
    uint16_t damage_dice_count;
    uint16_t clip_size;
    uint16_t high_capacity_dice_sides;
    uint16_t high_capacity_dice_count;
    int16_t modify_eject_ticks;
    int16_t modify_reload_ticks;
    int16_t modify_chamber_ticks;
    magazine_type_t type;
    weapon_type_t attaches_to;
    uint16_t incendiary_dice_sides;
    uint16_t incendiary_dice_count;
    uint16_t hv_dice_sides;
    uint16_t hv_dice_count;
    uint16_t tracer_dice_sides;
    uint16_t tracer_dice_count;
    uint16_t explosive_dice_sides;
    uint16_t explosive_dice_count;
  };
  struct underbarrel_data_t {
    enum underbarrel_type_t : uint16_t {
      UNDERB_T_FRAG_LAUNCHER = (1 << 0),
      UNDERB_T_INCENDIARY_FRAG_LAUNCHER = (1 << 1),
      UNDERB_T_SMOKE_LAUNCHER = (1 << 2),
      UNDERB_T_SHOTGUN = (1 << 3),
      UNDERB_T_FLAME_THROWER = (1 << 4),
      UNDERB_T_FLASH_BANG = (1 << 5),
    };
    uint64_t id;
    uint64_t created_at;
    char name[32];
    char manufacturer[32];
    char description[256];
    uint16_t damage_dice_sides;
    uint16_t damage_dice_count;
    uint16_t clip_size;
    int16_t modify_eject_ticks;
    int16_t modify_reload_ticks;
    int16_t modify_chamber_ticks;
    underbarrel_type_t type;
    uint16_t frag_dice_sides;
    uint16_t frag_dice_count;
    uint16_t incendiary_dice_sides;
    uint16_t incendiary_dice_count;
    uint16_t smoke_area_dice_sides;
    uint16_t smoke_area_dice_count;
    uint16_t smoke_duration_dice_sides;
    uint16_t smoke_duration_dice_count;
    uint16_t shotgun_dice_sides;
    uint16_t shotgun_dice_count;
    uint16_t shotgun_cone_shape_dice_sides;
    uint16_t shotgun_cone_shape_dice_count;
    uint16_t flame_dice_sides;
    uint16_t flame_dice_count;
    uint16_t flame_potency_dice_sides;
    uint16_t flame_potency_dice_count;
    uint16_t flash_duration_dice_sides;
    uint16_t flash_duration_dice_count;
  };
  struct muzzle_data_t {
    enum muzzle_type_t : uint16_t {
      MUZZLE_T_STABILIZER = (1 << 0),
      MUZZLE_T_SILENCER = (1 << 1),
      MUZZLE_T_REDUCE_RECOIL = (1 << 2),
      MUZZLE_T_COMPENSATOR = (1 << 3),
      MUZZLE_T_HEAT_RECYCLER = (1 << 4),
      MUZZLE_T_HEAT_DISSIPATION = (1 << 5),
    };
    uint64_t id;
    uint64_t created_at;
    char name[32];
    char manufacturer[32];
    char description[256];
    uint16_t stabilize_dice_sides;
    uint16_t stabilize_dice_count;
    uint16_t silence_dice_sides;
    uint16_t silence_dice_count;
    uint16_t reduce_recoil_dice_sides;
    uint16_t reduce_recoil_dice_count;
    uint16_t heat_threshold;
    uint16_t charges_per_recycle;
    uint16_t heat_dissipation_threshold;
    uint16_t heat_dissipation_cooldown;
    muzzle_type_t type;
  };
  using mag_list_t = std::forward_list<std::unique_ptr<magazine_data_t>>;
  using wpn_list_t = std::forward_list<std::unique_ptr<weapon_data_t>>;
  using ubar_list_t = std::forward_list<std::unique_ptr<underbarrel_data_t>>;
  static inline void list_contents(mag_list_t& data){
    for(const auto& wpn : data){
#ifdef WPN_LOADER_LIST_CONTENTS
      std::cout << "id: " << wpn->id << "\n";
      std::cout << "created_at: " << wpn->created_at << "\n";
      std::cout << "name: " << wpn->name << "\n";
      std::cout << "manufacturer: " << wpn->manufacturer << "\n";
      std::cout << "description: " << wpn->description << "\n";
      std::cout << "damage_dice_sides: '" << wpn->damage_dice_sides << "'\n";
      std::cout << "damage_dice_count: '" << wpn->damage_dice_count << "'\n";
      std::cout << "clip_size: '" << wpn->clip_size << "'\n";
      std::cout << "modify_eject_ticks: '" << wpn->modify_eject_ticks << "'\n";
      std::cout << "modify_reload_ticks: '" << wpn->modify_reload_ticks << "'\n";
      std::cout << "modify_chamber_ticks: '" << wpn->modify_chamber_ticks << "'\n";
      std::cout << "type: '" << wpn->type << "'\n";
      std::cout << "incendiary_dice_sides: '" << wpn->incendiary_dice_sides << "'\n";
      std::cout << "incendiary_dice_count: '" << wpn->incendiary_dice_count << "'\n";
      std::cout << "hv_dice_sides: '" << wpn->hv_dice_sides << "'\n";
      std::cout << "hv_dice_count: '" << wpn->hv_dice_count << "'\n";
      std::cout << "tracer_dice_sides: '" << wpn->tracer_dice_sides << "'\n";
      std::cout << "tracer_dice_count: '" << wpn->tracer_dice_count << "'\n";
      std::cout << "explosive_dice_sides: '" << wpn->explosive_dice_sides << "'\n";
      std::cout << "explosive_dice_count: '" << wpn->explosive_dice_count << "'\n";
#endif
    }
  }
  static inline void list_contents(const ubar_list_t& data){
    for(const auto& wpn : data){
#ifdef WPN_LOADER_LIST_CONTENTS
      std::cout << "id: " << wpn->id << "\n";
      std::cout << "created_at: " << wpn->created_at << "\n";
      std::cout << "name: " << wpn->name << "\n";
      std::cout << "manufacturer: " << wpn->manufacturer << "\n";
      std::cout << "description: " << wpn->description << "\n";
      std::cout << "damage_dice_sides: '" << wpn->damage_dice_sides << "'\n";
      std::cout << "damage_dice_count: '" << wpn->damage_dice_count << "'\n";
      std::cout << "clip_size: '" << wpn->clip_size << "'\n";
      std::cout << "modify_eject_ticks: '" << wpn->modify_eject_ticks << "'\n";
      std::cout << "modify_reload_ticks: '" << wpn->modify_reload_ticks << "'\n";
      std::cout << "modify_chamber_ticks: '" << wpn->modify_chamber_ticks << "'\n";
      std::cout << "type: '" << wpn->type << "'\n";
      std::cout << "incendiary_dice_sides: '" << wpn->incendiary_dice_sides << "'\n";
      std::cout << "incendiary_dice_count: '" << wpn->incendiary_dice_count << "'\n";
      std::cout << "smoke_area_dice_sides: '" << wpn->smoke_area_dice_sides << "'\n";
      std::cout << "smoke_area_dice_count: '" << wpn->smoke_area_dice_count << "'\n";
      std::cout << "smoke_duration_dice_sides: '" << wpn->smoke_duration_dice_sides << "'\n";
      std::cout << "smoke_duration_dice_count: '" << wpn->smoke_duration_dice_count << "'\n";
      std::cout << "shotgun_dice_sides: '" << wpn->shotgun_dice_sides << "'\n";
      std::cout << "shotgun_dice_count: '" << wpn->shotgun_dice_count << "'\n";
      std::cout << "shotgun_cone_shape_dice_sides: '" << wpn->shotgun_cone_shape_dice_sides << "'\n";
      std::cout << "shotgun_cone_shape_dice_count: '" << wpn->shotgun_cone_shape_dice_count << "'\n";
      std::cout << "flame_potency_dice_sides: '" << wpn->flame_potency_dice_sides << "'\n";
      std::cout << "flame_potency_dice_count: '" << wpn->flame_potency_dice_count << "'\n";
      std::cout << "flash_duration_dice_sides: '" << wpn->flash_duration_dice_sides << "'\n";
      std::cout << "flash_duration_dice_count: '" << wpn->flash_duration_dice_count << "'\n";
#endif
    }
  }
  static inline void list_contents(const wpn_list_t& data){
    for(const auto& wpn : data){
#ifdef WPN_LOADER_LIST_CONTENTS
      std::cout << "id: " << wpn->id << "\n";
      std::cout << "created_at: " << wpn->created_at << "\n";
      std::cout << "name: " << wpn->name << "\n";
      std::cout << "manufacturer: " << wpn->manufacturer << "\n";
      std::cout << "description: " << wpn->description << "\n";
      std::cout << "damage_dice_sides: '" << wpn->damage_dice_sides << "'\n";
      std::cout << "damage_dice_count: '" << wpn->damage_dice_count << "'\n";
      std::cout << "pixels_per_tick: '" << wpn->pixels_per_tick << "'\n";
      std::cout << "clip_size: '" << wpn->clip_size << "'\n";
      std::cout << "ammo_max: '" << wpn->ammo_max << "'\n";
      std::cout << "eject_ticks: '" << wpn->eject_ticks << "'\n";
      std::cout << "reload_ticks: '" << wpn->reload_ticks << "'\n";
      std::cout << "chamber_ticks: '" << wpn->chamber_ticks << "'\n";
      std::cout << "cooldown_between_shots_ticks: '" << wpn->cooldown_between_shots_ticks << "'\n";
      std::cout << "incendiary_base: '" << wpn->incendiary_base << "'\n";
      std::cout << "incendiary_dice_sides: '" << wpn->incendiary_dice_sides << "'\n";
      std::cout << "incendiary_dice_count: '" << wpn->incendiary_dice_count << "'\n";
      std::cout << "attached_scope_id: '" << wpn->attached_scope_id << "'\n";
      std::cout << "attached_underbarrel_id: '" << wpn->attached_underbarrel_id << "'\n";
      std::cout << "attached_magazine_id: '" << wpn->attached_magazine_id << "'\n";
      std::cout << "attached_barrel_id: '" << wpn->attached_barrel_id << "'\n";
      std::cout << "damage_decay: '" << wpn->damage_decay << "'\n";
      std::cout << "effective_range: '" << wpn->effective_range << "'\n";
#endif
    }
  }

  template <typename TData>
    struct weapon_loader_t {
      using ListType = std::forward_list<std::unique_ptr<TData>>;
      ListType data;
      weapon_loader_t(const weapon_loader_t& ) = delete;
      weapon_loader_t(FILE* handle, bool should_load){
        if(should_load){
          load(handle);
        }
      }
      int16_t load(FILE* handle) {
        int bytes = 0;
        do {
          std::unique_ptr<TData> temp_ptr = std::make_unique<TData>();
          bytes = fread(temp_ptr.get(),sizeof(TData),1,handle);
          if(bytes != 0){
            data.push_front(std::move(temp_ptr));
          }
        }while(bytes);
        return 0;
      }
      int16_t save(FILE* handle) {
        for(const auto& ptr : data){
          int bytes = fwrite(ptr.get(),sizeof(TData),1,handle);
          if(bytes == 0){
            std::cout << "Warning: failed to write: '" << ptr->name << "'\n";
          }
        }
        return 0;
      }
      void dump_contents(){
        list_contents(data);
      }

    };
  namespace vault {
    void init(int argc, char** argv);
  };
};
#endif
