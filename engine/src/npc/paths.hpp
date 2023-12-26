#ifndef __NPC_PATHS_HEADER__
#define __NPC_PATHS_HEADER__
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>
#include <array>
#include <vector>
#include "../actor.hpp"
#include "../world.hpp"
#include "../coordinates.hpp"
#include "../bullet-pool.hpp"
#include "../debug.hpp"
#include "../draw.hpp"
#include "../wall.hpp"

#include "../behaviour-tree.hpp"
#include "../npc-id.hpp"
#include "../direction.hpp"

//#define USE_DRAW_PATH

using point_t = std::tuple<std::pair<int32_t,int32_t>,bool>;
using vpair_t = std::pair<int32_t,int32_t>;
using Coordinate = vpair_t;
namespace npc::paths {
	static constexpr std::size_t DEMO_POINTS_SIZE = 10 * 1024;
	extern std::array<point_t,DEMO_POINTS_SIZE> demo_points;
	extern std::array<point_t,DEMO_POINTS_SIZE> gw_points;
	static constexpr std::size_t POINTS = 256;
	static constexpr std::size_t NEG_NORTH_WEST =0;
	static constexpr std::size_t NEG_NORTH =1;
	static constexpr std::size_t NEG_NORTH_EAST =2;
	static constexpr std::size_t NEG_EAST =3;
	static constexpr std::size_t NEG_SOUTH_EAST =4;
	static constexpr std::size_t NEG_SOUTH =5;
	static constexpr std::size_t NEG_SOUTH_WEST =6;
	static constexpr std::size_t NEG_WEST =7;
	bool has_line_of_sight(wall::Wall* from,wall::Wall* target);
	//bool has_line_of_sight(Actor* from,Actor* target);
	std::vector<vpair_t> getCoordinates(const vpair_t& point1, const vpair_t& point2, int distance);
	static inline auto distance(int32_t x1, int32_t y1, int32_t x2,int32_t y2) {
		auto dx{x1 - x2};
		auto dy{y1 - y2};
		return std::sqrt(dx*dx + dy*dy);
	}
	template <typename TRect>
	static inline auto distance(const TRect& src, const TRect& target) {
		auto dx{src->rect.x - target->rect.x};
		auto dy{src->rect.y - target->rect.y};
		return std::sqrt(dx*dx + dy*dy);
	}
	struct Score {
		int32_t x;
		int32_t y;
		uint32_t weight;
		uint32_t distance;
		bool visited;
		std::array<Score*,8> neighbors;
	};
	struct Point {
		int32_t x;
		int32_t y;
	};
	template <typename TContainer>
	void draw_path(const TContainer& points) {
		for(auto& l : demo_points) {
			l = {{0,0},false};
		}
		std::size_t i = 0;
		for(const auto& p : points) {
			demo_points[i++] = {vpair_t{p->rect.x,p->rect.y},true};
			if(i >= DEMO_POINTS_SIZE) {
				break;
			}
		}
	}
	struct Path {
		Point start;
		Point end;
		std::vector<Point> points;
	};
	struct ChosenPath {
		static constexpr std::size_t PATH_SIZE = 512;
		static constexpr std::size_t NTG_SIZE = 128;
		Direction calculate_heading();
		SDL_Point* next_point();
		SDL_Point current_point;
		bool traversal_ready;
		std::size_t path_elements;
		std::array<wall::Wall*,PATH_SIZE> path;
		std::size_t index;
		int32_t src_x;
		int32_t src_y;
		int32_t target_x;
		int32_t target_y;
		std::array<wall::Wall*,NTG_SIZE> nearest_target_gateways;
		ChosenPath();
		void generate();
		void generate(const int32_t& _src_x,
		              const int32_t& _src_y,
		              const int32_t& _target_x,
		              const int32_t& _target_y);
		void update(const Actor* src,const Actor* targ);
		void populate_nearest_target_gateways();

		std::vector<wall::Wall*> nearest_gateways(const int32_t& from_x,const int32_t& from_y);
		std::vector<wall::Wall*> obstacles;

		std::vector<wall::Wall*> right_angle_path(bool& unimpeded,bool swap);
		std::vector<wall::Wall*> direct_path_from(const int32_t& from_x,
		                                          const int32_t& from_y,
		                                          const int32_t& to_x,
		                                          const int32_t& to_y);
		uint16_t direct_path_from(std::vector<wall::Wall*> * storage);
		bool direct_path_tried;
		bool direct_path_unimpeded;

		void save_path(std::initializer_list<std::vector<wall::Wall*>> l);
		void save_path(const std::vector<wall::Wall*>& in_path);
		bool attempt_direct_path();
		bool attempt_right_angle();
		bool attempt_gateway_method();
		bool attempt_reverse_gateway();
		bool target_on_tile(wall::Wall* tile) const;
		bool target_close_to_tile(wall::Wall* tile) const;
		bool has_line_of_sight_from(wall::Wall* tile);

	};
	struct PathFinder {
			std::map<int32_t,wall::Wall*> get_closest_gateways(Actor* _in_target);
			std::unique_ptr<ChosenPath> chosen_path;
			int movement_amount;
			int x;
			int y;
			int target_x;
			int target_y;
			std::array<SDL_Point,POINTS> points;
			std::vector<SDL_Point*> obstacles;
			std::size_t valid_nodes;
			Direction heading;
			bool is_obstacle(const SDL_Point* _p);
			PathFinder() = delete;
			~PathFinder();
			PathFinder(int hunter_movement_amount, Actor* _in_hunter,Actor* _in_target);
			void update(Actor* hunter,Actor* target);
			void crawl_map_tiles();
			void draw_path();
			void animate();
			bool rerouting;
			SDL_Point* next_point();

		private:
			SDL_Point m_current_point;
			Actor* m_hunter;
			Actor* m_target;
			std::vector<wall::Wall*> m_path;
			vpair_t start_point;
			vpair_t destination_point;
			/**
			 * It's unavoidable: We _MUST_ implement Dijkstra's algorithm
			 */
			std::map<vpair_t,Score> m_sample_grid;
			std::vector<std::vector<Score>> m_grid;
			std::vector<Path> m_paths;
	};
	wall::Wall* get_tile(const int32_t& x,const int32_t& y);
	wall::Wall* get_tile(const vpair_t& _src);
	wall::Wall* get_tile(Actor* a);
};
#endif
