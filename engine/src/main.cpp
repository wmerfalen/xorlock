#define DEVELOPMENT_MENU 1
#include <cstdlib>
#include <iostream>
#include <SDL2/SDL_mixer.h>
#include "filesystem.hpp"
extern uint64_t CURRENT_TICK;
#include "defines.hpp"
#include "world.hpp"
#include "background.hpp"
#include "player.hpp"
#include "draw-state/init.hpp"
#include "draw-state/ammo.hpp"
#include "draw-state/backpack.hpp"
#include "movement.hpp"
#include "triangle.hpp"
#include "bullet-pool.hpp"
#include "npc-spetsnaz.hpp"
#include "npc-bomber.hpp"
#include "cursor.hpp"
#include "tick.hpp"
#include "viewport.hpp"
#include "timeline.hpp"
#include "map.hpp"
#include "randomized-maps/building-generator.hpp"
#include "gameplay.hpp"
#include "reload.hpp"
#include "weapons/weapon-loader.hpp"
#include "font.hpp"
#include "db.hpp"
#include "sound/gunshot.hpp"
#include "sound/reload.hpp"
#include "sound/npc.hpp"
#include "sound/menu.hpp"
#include "time.hpp"
#include "air-support/f35.hpp"
#include "damage/explosions.hpp"
#include "weapons/grenade.hpp"
#include "ability.hpp"
#include "abilities/turret.hpp"
#include "events/death.hpp"
#include "backpack.hpp"
// FIXME
//#define DRAW_GATEWAYS
#include "wall.hpp"

std::unique_ptr<Player> guy = nullptr;
std::unique_ptr<World> world = nullptr;
std::unique_ptr<MovementManager> movement_manager = nullptr;
#ifdef REPORT_ERROR
#undef REPORT_ERROR
#endif


#ifdef SHOW_ERRORS
#define REPORT_ERROR(A) std::cerr << "[ERROR][" << __FUNCTION__ << "@" << __FILE__ << ":" << __LINE__ << "]: " << A << "\n";
#else
#define REPORT_ERROR(A)
#endif

#undef m_debug
#undef m_error
#define m_debug(A) std::cout << "[main.cpp][DEBUG]: " << A << "\n";
#define m_error(A) std::cout << "[main.cpp][ERROR]: " << A << "\n";
#ifdef SHOW_MOUSE_DEBUG
#define mouse_debug(A) std::cout << "[main.cpp][MOUSE_DEBUG]: " << A << "\n";
#else
#define mouse_debug(A) /*-*/
#endif
extern std::vector<SDL_Surface*> surface_list;
extern std::vector<SDL_Texture*> texture_list;
extern std::vector<size_t> rendered;
// TODO: allow user to dynamically change these
extern int WIN_WIDTH; // defined in window.cpp
extern int WIN_HEIGHT; // defined in window.cpp

extern std::size_t initial_load_count;
extern std::size_t total_call_count;
extern std::size_t cached_call_count;
extern std::size_t cache_missed_count;
extern void report_world();
SDL_Window* win = nullptr;
int32_t START_X = WIN_WIDTH / 2;
int32_t START_Y = WIN_HEIGHT / 2;
uint64_t render_time;
bool do_draw_last = true;
SDL_Rect draw_last_rect;
std::string draw_last_msg;
int draw_last_height = 80;
int draw_last_width = 60;
SDL_Point draw_last_point;
namespace npc::paths {
  extern void report();
};
void draw_last(){
#ifdef DRAW_LAST_EXTRA_INFO
  draw::line(guy->cx,guy->cy,cursor::mx(),cursor::my()); 
  if((tick::get() % 100) == 0){
    std::cout << "initial_load_count: " << initial_load_count << "\n";
    std::cout << "total_call_count: " << total_call_count << "\n";
    std::cout << "cached_call_count: " << cached_call_count << "\n";
    std::cout << "cache_missed_count: " << cache_missed_count << "\n";
    report_world();
    npc::paths::report();
  }
  SDL_Point tile_coord_point{0,250};
  auto tile = npc::paths::get_tile(&guy->self);
  if(tile){
    std::string s = std::to_string(tile->index);
    font::green_text(&tile_coord_point,s,50,250);
    tile_coord_point.y = WIN_HEIGHT - 30;
    s = std::to_string(tile->orig_rect.x) + " x " + std::to_string(tile->orig_rect.y);
    font::small_red_text(&tile_coord_point,s,30);

    tile_coord_point.y = WIN_HEIGHT - 30;
    tile_coord_point.x = WIN_WIDTH / 2;
    s = std::to_string(tile->rect.x) + " x " + std::to_string(tile->rect.y);
    font::small_red_text(&tile_coord_point,s,30);
  }
#endif
  if(!do_draw_last){
    return;
  }
  draw::blatant_rect(&draw_last_rect);
  if(draw_last_msg.length()){
    font::green_text(&draw_last_point,draw_last_msg,draw_last_height,draw_last_width);
  }
}
void ren_clear() {
  SDL_RenderClear(ren);
}
void ren_present() {
  SDL_RenderPresent(ren);
}
int tile_width() {
  return WIN_WIDTH / 32;
}
int win_width() {
  return WIN_WIDTH;
}
int win_height() {
  return WIN_HEIGHT;
}
SDL_Event event;
SDL_Renderer* ren;
SDL_bool done = SDL_FALSE;
SDL_bool new_game = SDL_TRUE;
void setup_event_filter() {
  std::vector<uint32_t> disable = {
    SDL_APP_TERMINATING,
    SDL_APP_LOWMEMORY,
    SDL_APP_WILLENTERBACKGROUND,
    SDL_APP_DIDENTERBACKGROUND,
    SDL_APP_WILLENTERFOREGROUND,
    SDL_APP_DIDENTERFOREGROUND,
    SDL_LOCALECHANGED,
    SDL_DISPLAYEVENT,
    SDL_WINDOWEVENT,
    SDL_SYSWMEVENT,
    SDL_TEXTEDITING,
    SDL_TEXTINPUT,
    SDL_KEYMAPCHANGED,
    SDL_JOYAXISMOTION,
    SDL_JOYBALLMOTION,
    SDL_JOYHATMOTION,
    SDL_JOYBUTTONDOWN,
    SDL_JOYBUTTONUP,
    SDL_JOYDEVICEADDED,
    SDL_JOYDEVICEREMOVED,
    SDL_CONTROLLERAXISMOTION,
    SDL_CONTROLLERBUTTONDOWN,
    SDL_CONTROLLERBUTTONUP,
    SDL_CONTROLLERDEVICEADDED,
    SDL_CONTROLLERDEVICEREMOVED,
    SDL_CONTROLLERDEVICEREMAPPED,
    SDL_FINGERDOWN,
    SDL_FINGERUP,
    SDL_FINGERMOTION,
    SDL_DOLLARGESTURE,
    SDL_DOLLARRECORD,
    SDL_MULTIGESTURE,
    SDL_CLIPBOARDUPDATE,
    SDL_DROPFILE,
    SDL_DROPTEXT,
    SDL_DROPBEGIN,
    SDL_DROPCOMPLETE,
    SDL_AUDIODEVICEADDED,
    SDL_AUDIODEVICEREMOVED,
    SDL_RENDER_TARGETS_RESET,
    SDL_RENDER_DEVICE_RESET,
  };
  std::vector<uint32_t> enable = {
    SDL_KEYDOWN,
    SDL_KEYUP,
    SDL_MOUSEMOTION,
    SDL_MOUSEBUTTONDOWN,
    SDL_MOUSEBUTTONUP,
    SDL_MOUSEWHEEL,
    //SDL_USEREVENT,
    //SDL_LASTEVENT,
  };
  for(const auto& d : disable) {
    SDL_EventState(d,SDL_DISABLE);
  }
  for(const auto& e : enable) {
    SDL_EventState(e,SDL_ENABLE);
  }

}
#ifdef DEVELOPMENT_MENU
bool dev_menu() {
  auto r = plr::get_effective_rect();
  std::cout << dbg::dump(r) << "\n";
  std::cout << plr::self()->world_x << "x" << plr::self()->world_y << "\n";
  return true;
}
#endif
int numkeys = 255;
const Uint8* keys;
static constexpr uint8_t KEY_W = 26;
static constexpr uint8_t KEY_A = 4;
static constexpr uint8_t KEY_S = 22;
static constexpr uint8_t KEY_D = 7;
static constexpr uint8_t KEY_R = 21;
static constexpr uint8_t KEY_NUM_1 = SDL_SCANCODE_1;
static constexpr uint8_t KEY_NUM_2 = SDL_SCANCODE_2;
static constexpr uint8_t KEY_NUM_3 = SDL_SCANCODE_3;
static constexpr uint8_t KEY_NUM_4 = SDL_SCANCODE_4;
static constexpr uint8_t KEY_NUM_5 = SDL_SCANCODE_5;
static constexpr uint8_t KEY_NUM_6 = SDL_SCANCODE_6;
static constexpr uint8_t KEY_NUM_7 = SDL_SCANCODE_7;
static constexpr uint8_t KEY_NUM_8 = SDL_SCANCODE_8;
static constexpr uint8_t KEY_NUM_9 = SDL_SCANCODE_9;
static constexpr uint8_t KEY_NUM_0 = SDL_SCANCODE_0;
static constexpr uint8_t SPACE_BAR = 44;
static constexpr uint8_t ESCAPE = SDL_SCANCODE_ESCAPE;
uint64_t escape_window = 0;
bool is_paused = false;
std::vector<loot::Loot*> loot_nearby;
std::vector<SDL_Rect> loot_memory;
uint64_t tab_window = 0;
uint64_t pickup_window = 0;
#ifdef TEST_DROPS
uint64_t drop_window = 0;
#endif
#ifdef TEST_TURRET
uint64_t turret_window = 0;
#endif
#ifdef TEST_NPC_BOMBERS
uint64_t bomber_window = 0;
#endif
namespace npc {
  extern int spetsnaz_mode;
};
void handle_movement() {
  keys = SDL_GetKeyboardState(nullptr);
#ifdef TEST_TURRET
  if(keys[SDL_SCANCODE_SPACE]){
    if(turret_window <= tick::get()){
      abilities::turret::spawn(plr::get()->cx,plr::get()->cy);
      turret_window = tick::get() + 50;
    }
    return;
  }
#endif
#ifdef TEST_NPC_BOMBERS
  if(keys[SDL_SCANCODE_SPACE]){
    if(bomber_window <= tick::get()){
      npc::bomber::spawn_bomber(4);
      bomber_window = tick::get() + 50;
    }
    return;
  }
#endif
#ifdef TEST_DROPS
  if(keys[SDL_SCANCODE_SPACE]){
    if(drop_window <= tick::get()){
      npc::spetsnaz_mode += 1;
      events::death::dispatch(constants::npc_type_t::NPC_SPETSNAZ, -1, plr::cx(),plr::cy());
      drop_window = tick::get() + 1000;
    }
    return;
  }
  if(keys[SDL_SCANCODE_BACKSPACE]){
    if(drop_window <= tick::get()){
      npc::spetsnaz_mode -= 1;
      events::death::dispatch(constants::npc_type_t::NPC_SPETSNAZ, -2, plr::cx(),plr::cy());
      drop_window = tick::get() + 1000;
    }
    return;
  }
  if(keys[SDL_SCANCODE_BACKSLASH]){
    if(drop_window <= tick::get()){
      events::death::dispatch(constants::npc_type_t::NPC_SPETSNAZ, -3, plr::cx(),plr::cy());
      drop_window = tick::get() + 1000;
    }
    return;
  }
  if(keys[SDL_SCANCODE_DELETE]){
    if(drop_window <= tick::get()){
      events::death::dispatch(constants::npc_type_t::NPC_SPETSNAZ, -4, plr::cx(),plr::cy());
      drop_window = tick::get() + 1000;
    }
    return;
  }
#endif

  if(keys[SDL_SCANCODE_TAB]){
    if(tab_window <= tick::get()){
      draw_state::backpack::show_backpack();
      draw_state::backpack::start_menu();
      tab_window = tick::get() + 1000;
      do {
        ren_clear();
        draw_state::backpack::draw_menu(ren);
        SDL_RenderPresent(ren);
        ::usleep(25000);
      } while(draw_state::backpack::draw_backpack());
    }
  }


  if(keys[SPACE_BAR] && !done && !new_game){
    air_support::f35::space_bar_pressed();
  }
#if 0
#ifdef DEVELOPMENT_MENU
  if(keys[SPACE_BAR]) {
    if(!dev_menu()) {
      return;
    }
  }
#endif
#endif
  //bool num_1 = keys[KEY_NUM_1];
  //bool num_2 = keys[KEY_NUM_2];
  bool north_east = keys[KEY_W] && keys[KEY_D];
  bool north_west = keys[KEY_W] && keys[KEY_A];
  bool south_east = keys[KEY_S] && keys[KEY_D];
  bool south_west = keys[KEY_S] && keys[KEY_A];
  bool north = keys[KEY_W] && (!keys[KEY_D] && !keys[KEY_A]);
  bool south = keys[KEY_S] && (!keys[KEY_D] && !keys[KEY_A]);
  bool east = keys[KEY_D] && (!keys[KEY_W] && !keys[KEY_S]);
  bool west = keys[KEY_A] && (!keys[KEY_W] && !keys[KEY_S]);
  bool reload_key_pressed = keys[KEY_R];
  if(keys[ESCAPE] && escape_window < CURRENT_TICK){
    escape_window = CURRENT_TICK + 2000;
    gameplay::toggle_menu();
    sound::pause_music();
    do {
      ren_clear();
      gameplay::draw_pause_menu(ren);
      SDL_RenderPresent(ren);
      ::usleep(25000);
    } while(gameplay::game_is_paused());
    if(gameplay::wants_quit()){
      done = SDL_TRUE;
      return;
    }
    if(gameplay::wants_new_game()){
      new_game = SDL_TRUE;
      return;
    }
    sound::resume_music();
    return;
  }
  SDL_Keymod mod = SDL_GetModState();
  if(mod == KMOD_LSHIFT) {
    plr::run(true);
  } else {
    plr::run(false);
  }
  bool num_1 = keys[KEY_NUM_1];
  bool num_2 = keys[KEY_NUM_2];
  bool num_3 = keys[KEY_NUM_3];
  bool num_4 = keys[KEY_NUM_4];
  bool num_5 = keys[KEY_NUM_5];
  bool num_6 = keys[KEY_NUM_6];
  bool num_7 = keys[KEY_NUM_7];
  bool num_8 = keys[KEY_NUM_8];
  bool num_9 = keys[KEY_NUM_9];
  bool num_0 = keys[KEY_NUM_0];
  if(num_1){
    m_debug("num 1");
    guy->start_equip_weapon(0);
  }else if(num_2){
    m_debug("num 2");
    guy->start_equip_weapon(1);
  }else if(num_3){
    m_debug("num 3");
    guy->start_equip_weapon(2);
  }

  //if(gameplay::needs_numeric()) {
  //}
  if(reload_key_pressed && guy->reloader->is_reloading() == false) {
    reload::reload_response_t response = guy->reloader->start_reload();
    switch(response) {
      case reload::reload_response_t::NOT_ENOUGH_AMMO:
        std::cout << "not enough ammo\n";
        break;
      case reload::reload_response_t::CURRENTLY_RELOADING:
        std::cout << "currently reloading\n";
        break;
      case reload::reload_response_t::STARTING_RELOAD:
        std::cout << "Starting reload\n";
        break;
      case reload::reload_response_t::CLIP_FULL:
        std::cout << "clip full\n";
        break;
      default:
        std::cout << "default\n";
        break;
    }
  }
  if(north_east) {
    movement_manager->wants_to_move(*world,NORTH_EAST);
  } else if(north_west) {
    movement_manager->wants_to_move(*world,NORTH_WEST);
  } else if(south_east) {
    movement_manager->wants_to_move(*world,SOUTH_EAST);
  } else if(south_west) {
    movement_manager->wants_to_move(*world,SOUTH_WEST);
  } else if(north) {
    movement_manager->wants_to_move(*world,NORTH);
  } else if(south) {
    movement_manager->wants_to_move(*world,SOUTH);
  } else if(east) {
    movement_manager->wants_to_move(*world,EAST);
  } else if(west) {
    movement_manager->wants_to_move(*world,WEST);
  }

  guy->calc();
  // TODO: #define this out
#if 0
  {
    SDL_Point p{250,0};
    std::string m = std::to_string(guy->cx);
    m += "x";
    m += std::to_string(guy->cy);
	  font::small_red_text(&p,//const SDL_Point* where,
                            m,//const std::string& msg,
                            90);  //int height);
  }
  {
    SDL_Point p{250,150};
    std::string m = std::to_string(cursor::mx());
    m += "x";
    m += std::to_string(cursor::my());
	  font::small_red_text(&p,//const SDL_Point* where,
                            m,//const std::string& msg,
                            90);  //int height);
  }
  {
    auto angle = coord::get_angle(guy->cx,guy->cy,cursor::mx(),cursor::my());
    SDL_Point p{250,250};
    std::string m = std::to_string(angle);
	  font::small_red_text(&p,//const SDL_Point* where,
                            m,//const std::string& msg,
                            90);  //int height);
  }
#endif
  do_draw_last = false;
  auto nearby_loot = loot::near_loot(plr::get_rect());
  for(auto& loot : nearby_loot){
    do_draw_last = true;
    SDL_Rect r;
    r.x = loot->where.x - 40;
    r.y = loot->where.y - 40;
    r.w = 160;
    r.h = 160;
    draw_last_rect = r;
    draw_last_point.x = r.x - 100;
    draw_last_point.y = r.y - 100;
    draw_last_msg = loot->name;
    draw_last_height = 25;
    draw_last_width = 300;
  }
  if(keys[SDL_SCANCODE_E] && pickup_window + 100 < tick::get() && nearby_loot.size()){
    loot::pickup_loot(nearby_loot[0]);
    pickup_window = tick::get();
  }
}
bool handle_mouse() {
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_MOUSEBUTTONDOWN:
        mouse_debug("mouse down");
        if(guy->reloader->is_reloading() == false) {
          mouse_debug("mouse down - start_gun okay");
          plr::start_gun();
        }
        break;
      case SDL_MOUSEBUTTONUP:
        plr::stop_gun();
        break;
      case SDL_MOUSEMOTION:
        cursor::update_mouse();
        plr::rotate_guy();
        break;
      case SDL_MOUSEWHEEL:
        if(event.wheel.y > 0){ // Scroll up
          mouse_debug("mwheel up");
          guy->cycle_previous_weapon();
        }else if(event.wheel.y < 0){
          mouse_debug("mwheel down");
          guy->cycle_next_weapon();
        }
        break;
      case SDL_QUIT:
        done = SDL_TRUE;
        return false;
      default:
        break;
    }
  }
  return true;
}
std::string level_csv;
int main(int argc, char** argv) {
  static constexpr const char* title = "Xorlock v0.3.0";
  if(argc > 1){
    for(size_t i=1; i < argc; i++){
      std::string arg = argv[i];
      if(arg.find("--level=") != std::string::npos){
        level_csv = constants::assets_dir;
        level_csv += arg.substr(strlen("--level="));
      }
    }
  }
  if(level_csv.length() == 0){
    level_csv = constants::assets_dir;
    level_csv += "apartment-blockade-0.csv";
  }
  if(!fs::exists(level_csv)){
    std::cout << "ERROR: couldn't open level file: '" << level_csv << "'\n" <<
      "Exiting now...\n";
    return 1;
  }

  if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    REPORT_ERROR("SDL_Init Error: " << SDL_GetError());
    return EXIT_FAILURE;
  }
  if(SDL_Init(SDL_INIT_AUDIO) != 0){
    REPORT_ERROR("SDL_Init Error: " << SDL_GetError());
    return EXIT_FAILURE;
  }

  START_X = WIN_WIDTH / 2;
  START_Y = WIN_HEIGHT / 2;

  win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
  if(win == nullptr) {
    REPORT_ERROR("SDL_CreateWindow Error: " << SDL_GetError());
    return EXIT_FAILURE;
  }

  ren
    = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if(ren == nullptr) {
    REPORT_ERROR("SDL_CreateRenderer Error" << SDL_GetError());
    if(win != nullptr) {
      SDL_DestroyWindow(win);
    }
    SDL_Quit();
    return EXIT_FAILURE;
  }
  std::cout << "START_X: " << START_X << " " << "START_Y: " << START_Y << "\n";
  rng::init();

  init_world(level_csv);
  world = std::make_unique<World>();
  wall::init();
  guy = std::make_unique<Player>(world->start_tile_x(),world->start_tile_y(),"../assets/guy-0.bmp", BASE_MOVEMENT_AMOUNT);
  movement_manager = std::make_unique<MovementManager>();
  backpack::init();
  events::death::init();
  loot::init();
  ability::init();
  abilities::turret::init();
  sound::init();
  sound::reload::init();
  sound::npc::init();
  sound::menu::init();
  air_support::f35::init();
  db::init();
  bg::init();
  plr::set_guy(guy.get());
  wpn::vault::init(argc,argv); // Defined in weapons/weapon-loader.hpp
  guy->equip_weapon(0);
  bg::draw();
  cursor::init();
  cursor::use_reticle();
  font::init();
  viewport::init();
  setup_event_filter();
  bullet::init();
  timeline::init();
  draw_state::init();
  map::init();
  rmapgen::init();
  gameplay::init();
  npc::init_spetsnaz();
  npc::bomber::init();
  movement::init(movement_manager.get());
  draw_state::ammo::init();
  draw_state::player::init();
  damage::explosions::init();
  weapons::grenade::init();
  static constexpr uint32_t target_render_time = 25000;
  new_game = SDL_FALSE;
#ifdef NO_MUSIC
#else
  sound::start_track("track-01-camo");
#endif

  while(!done) {
    timeline::start_timer();
    ren_clear();
    handle_mouse();
    handle_movement();
#ifdef F35_STRESS_TEST
    air_support::f35::space_bar_pressed();
#endif

    plr::tick();
    ability::tick();
    map::tick();
    timeline::tick();
    if(guy->reloader->is_reloading()) {
      plr::update_reload_state(guy->reloader->tick());
    }
    bullet::draw_shells();
    plr::redraw_guy();
    npc::spetsnaz_tick();
    npc::bomber::tick(); // FYI: preferred namespace schema
    plr::draw_reticle();
    draw::blatant();
    draw::overlay_grid();
    gameplay::tick();
    draw::tick_timeline();
    air_support::f35::tick();
    abilities::turret::tick();
    damage::explosions::tick();
    weapons::grenade::tick();
    events::death::tick();
    loot::tick();
    bullet::tick();
    draw_state::player::tick();
    draw_last();
    draw_state::backpack::tick();
    SDL_RenderPresent(ren);
    render_time = timeline::stop_timer();
    if(render_time < target_render_time) {
      ::usleep(target_render_time - render_time);
    }
    std::cout << std::flush;
  }
  backpack::program_exit();
  wall::program_exit();
  world_program_exit(); // sets world=nullptr, among other things...
  sound::program_exit(); // see sound/gunshot.hpp
  actor_program_exit();
  sound::npc::program_exit();
  sound::reload::program_exit();
  sound::menu::program_exit();
  bg::program_exit();
  gameplay::program_exit();
  damage::explosions::program_exit();
  air_support::f35::program_exit();
  font::quit();
  bullet::program_exit();
  events::death::program_exit();
  loot::program_exit();
  movement_manager = nullptr;
  guy->reloader = nullptr;
  guy = nullptr;
  Mix_HaltChannel(-1);
  Mix_HaltMusic();

  Mix_CloseAudio();
  // force a quit
  while(Mix_Init(0)){
    Mix_Quit();
  }
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return EXIT_SUCCESS;
}
