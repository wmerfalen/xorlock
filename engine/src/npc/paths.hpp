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
#include "../mp5.hpp"

#include "../behaviour-tree.hpp"
#include "../npc-id.hpp"
#include "../direction.hpp"

namespace npc::paths {
	static constexpr std::size_t POINTS = 256;
	static constexpr std::size_t NEG_NORTH_WEST =0;
	static constexpr std::size_t NEG_NORTH =1;
	static constexpr std::size_t NEG_NORTH_EAST =2;
	static constexpr std::size_t NEG_EAST =3;
	static constexpr std::size_t NEG_SOUTH_EAST =4;
	static constexpr std::size_t NEG_SOUTH =5;
	static constexpr std::size_t NEG_SOUTH_WEST =6;
	static constexpr std::size_t NEG_WEST =7;
	struct Score {
		int32_t x;
		int32_t y;
		uint32_t weight;
		uint32_t distance;
		bool visited;
		std::array<Score*,8> neighbors;
	};
	using vpair_t = std::pair<int32_t,int32_t>;
	struct PathFinder {
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
			~PathFinder() = default;
			PathFinder(int hunter_movement_amount, Actor* _in_hunter,Actor* _in_target);
			void update(Actor* hunter,Actor* target);
			const std::size_t& calculate_path();
			Direction calculate_heading();
			void animate();
			bool rerouting;

		private:
			uint32_t x_distance;
			uint32_t y_distance;
			vpair_t start_point;
			vpair_t destination_point;
			/**
			 * It's unavoidable: We _MUST_ implement Dijkstra's algorithm
			 */
			std::map<vpair_t,Score> m_sample_grid;
			std::vector<std::vector<Score>> m_grid;
	};
};
#endif
