#ifndef __BULLET_HEADER__
#define __BULLET_HEADER__
#include <SDL2/SDL.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <memory>
#include <iomanip>
#include "actor.hpp"
#include "triangle.hpp"
#include "coordinates.hpp"
#include "viewport.hpp"
#include "circle.hpp"
#include <vector>
#include <deque>
#include "clock.hpp"
#include "rng.hpp"
#include "draw.hpp"
#include "line.hpp"
#include "npc-id.hpp"


namespace bullet {
	void cleanup_pool();
  void draw_shell_at(const uint32_t& x,const uint32_t& y,const uint32_t& wpn_type);

	struct Point {
		int x;
		int y;
	};


	struct Bullet {
		//static constexpr int radius = 55;
		uint32_t start_tick;
    enum queue_type_t : uint16_t {
      QUEUE_TYPE_NPC,
      QUEUE_TYPE_PLAYER,
      QUEUE_TYPE_TURRET,
    };
    queue_type_t queue_type;
		bool is_npc;
		SDL_Rect rect;
		Point src;
		Point dst;
		Point current;
		weapon_stats_t* stats;
		Line line;
		std::size_t line_index;
		int angle;
		npc_id_t npc_id;

		std::vector<Point> trimmed;
		std::vector<SDL_Point> circle_points;
		int distance;
		int closest;
		bool done;
		Bullet();
		Bullet(const Bullet& o) = delete;
		~Bullet() = default;
		void calc();
		bool needs_processing();
		SDL_Texture* bullet_trail_texture();
		void draw_bullet_trail();
		void travel();

		void report();
	};
	struct BulletPool {
#ifdef BULLET_POOL_CUSTOM_SIZE
		static constexpr std::size_t POOL_SIZE = BULLET_POOL_CUSTOM_SIZE;
#else
		static constexpr std::size_t POOL_SIZE = 256; // TODO: scale this according to the user's computing power
#endif
		std::size_t index;
		std::array<std::unique_ptr<Bullet>,POOL_SIZE> bullets;
		~BulletPool() = default;
		BulletPool(const BulletPool&) = delete;
		BulletPool();
		void queue(weapon_stats_t* stats_ptr);
    void queue_custom(weapon_stats_t* stats_ptr,Bullet::queue_type_t t,int in_cx,int in_cy,int in_dst_x,int in_dst_y,npc_id_t nid);
    void queue_npc(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx, int in_cy,int dest_x,int dest_y);
  };
  void queue_custom(weapon_stats_t* stats_ptr,Bullet::queue_type_t t,int in_cx,int in_cy,int in_dst_x,int in_dst_y,npc_id_t nid);
  void queue_bullets(weapon_stats_t* stats_ptr);
  void queue_npc_bullets(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx,int in_cy,int dest_x, int dest_y);
  void tick();
  void init();
  void cleanup_pool();
  void program_exit();
  void move_map(int dir,int amount);
  void draw_shells();
};

#endif
