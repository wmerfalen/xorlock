#include "bullet.hpp"
#include "tick.hpp"
#include "world.hpp"
#include "npc-spetsnaz.hpp"

namespace bullet {
	void Bullet::clear() {
		done = true;
		initialized = false;
		trimmed.clear();
		line.points.clear();
	}
	void Bullet::calc() {
		clear();

		start_tick = tick::get();
		distance = closest = 9999;
		line_index = 0;
		angle = coord::get_angle(src.x,src.y,dst.x,dst.y);
		line.p1.x = src.x;
		line.p1.y = src.y;
		line.p2.x = (1000 * win_width()) * cos(PI * 2  * angle / 360);
		line.p2.y = (1000 * win_height()) * sin(PI * 2 * angle / 360);
		line.getPoints(INITIAL_POINTS);
		circle_points = shapes::CaptureDrawCircle(src.x,src.y, radius);
		for(const auto& cp : circle_points) {
			for(const auto& pt : line.points) {
				distance = sqrt(pow(pt.x - cp.x,2) + pow(pt.y - cp.y, 2) * 1.0);
				if(distance < closest) {
					line.p1.x = pt.x;
					line.p1.y = pt.y;
					closest = distance;
					if(distance < 60) {
						break;
					}
				}
			}
		}
		for(const auto& point : line.points) {
			if(point.x < viewport::min_x || point.x > viewport::max_x ||
			        point.y < viewport::min_y || point.y > viewport::max_y) {
				dst.x = line.p2.x = point.x;
				dst.y = line.p2.y = point.y;
				break;
			}
		}
		line.getPoints(INITIAL_POINTS);
		rect.x = line.p1.x;
		rect.y = line.p1.y;
		current.x = line.p1.x;
		current.y = line.p1.y;
		for(const auto& p : line.points) {
			distance = sqrt(pow(current.x - p.x,2) + pow(current.y - p.y, 2) * 1.0);
			if(distance >= (*stats)[WPN_PIXELS_PT]) {
				trimmed.emplace_back();
				auto& r = trimmed.back();
				r.x = p.x;
				r.y = p.y;
				current.x = p.x;
				current.y = p.y;
			}
		}
		current.x = line.p1.x;
		current.y = line.p1.y;
		line.points.clear();
		initialized = true;
	}
	bool Bullet::needs_processing() {
		return !done && initialized;
	}
	SDL_Texture* Bullet::bullet_trail_texture() {
		return bullet_trail.bmp[0].texture;
	}
	void Bullet::draw_bullet_trail() {
		draw::bullet_line(
		    src.x,  //int x
		    src.y,  //int y
		    rect.x, //int tox
		    rect.y  //,int toy) {
		);
	}
	void Bullet::travel() {
		if(line_index >= trimmed.size() - 1) {
			clear();
			return;
		}
		rect.x = trimmed[line_index].x;
		rect.y = trimmed[line_index].y;
		SDL_RenderCopy(
		    ren,
		    bullet_trail.bmp[0].texture,
		    nullptr,
		    &rect);
		SDL_Rect result;

		bool impact = 0;
		if(is_npc) {
			if(SDL_IntersectRect(
			            &rect,
			            plr::get_rect(),
			            &result)) {
				plr::take_damage(stats);
				impact = 1;
			}
		} else {
			for(auto& npc : world->npcs) {
				if(SDL_IntersectRect(
				            &rect,
				            &npc->rect,
				            &result)) {
					npc::take_damage(npc,plr::gun_damage());
					impact = 1;
				}
			}
		}
		draw_bullet_trail();
		if(impact) {
			clear();
			return;
		}
		current.x = rect.x;
		current.y = rect.y;
		++line_index;
	}

	void Bullet::report() {
		std::cout << "Bullet: " << src.x << "x" << src.y <<
		          "=>" << dst.x << "x" << dst.y << "\n";
	}
	void BulletPool::queue(weapon_stats_t* stats_ptr) {
		if(index >= POOL_SIZE -1) {
			index = 0;
		}

		auto& r = this->bullets[index];
		r->stats = stats_ptr;
		r->src.x = plr::get_cx();
		r->src.y = plr::get_cy();
		r->dst.x = cursor::mx();
		r->dst.y = cursor::my();
		r->is_npc = false;
		r->calc();
		r->done = false;
		r->initialized = true;
		++index;
	}
	void BulletPool::queue_npc(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx, int in_cy,int dest_x,int dest_y) {
		if(index >= POOL_SIZE -1) {
			index = 0;
		}

		auto& r = this->bullets[index];
		r->npc_id = in_npc_id;
		r->is_npc = true;
		r->stats = stats_ptr;
		r->src.x = in_cx;
		r->src.y = in_cy;
		r->dst.x = dest_x;
		r->dst.y = dest_y;
		r->calc();
		r->done = false;
		r->initialized = true;
		++index;
	}
	void queue_bullets(weapon_stats_t* stats_ptr) {
		pool->queue(stats_ptr);
	}
	void queue_npc_bullets(const npc_id_t& in_npc_id,weapon_stats_t* stats_ptr,int in_cx,int in_cy,int dest_x, int dest_y) {
		pool->queue_npc(in_npc_id,stats_ptr,in_cx,in_cy,dest_x,dest_y);
	}
	void tick() {
		for(auto& bullet : pool->bullets) {
			if(bullet->needs_processing()) {
				//if(bullet->start_tick + 600 >= tick::get()) {
				bullet->travel();
				//} else {
				//	bullet->clear();
				//}
			}
		}
	}
	void init() {
		bullet_trail.x = 0;
		bullet_trail.y = 0;
		bullet_trail.load_bmp_asset("../assets/bullet-trail-component-0.bmp");
		radius = 55;
		pool = std::make_unique<BulletPool>();
	}
	void cleanup_pool() {
	}
};