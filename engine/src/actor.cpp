#include "actor.hpp"
#include "asset.hpp"

#ifdef FAIL
#undef FAIL
#endif

#ifdef DEBUG
#undef DEBUG
#endif

#ifdef SHOW_FAILS
#define FAIL(A) std::cerr << "FAIL(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
#else
#define FAIL(A)
#endif

#define DEBUG(A) std::cout << "DEBUG(" << __FUNCTION__  << ":" << __LINE__ << "): " << A << "\n";
extern SDL_Renderer* ren;
Actor::Actor() {
	ready = 0;
}
Actor::Actor(int32_t _x,int32_t _y,const char* _bmp_path) {
	this->x = (_x);
	this->y = (_y);
	this->rect = SDL_Rect{_x,_y,68,38};
	if(std::string(_bmp_path).find_first_of("%d") != std::string::npos) {
		load_bmp_assets(_bmp_path,360);
	} else {
		auto p = SDL_LoadBMP(_bmp_path);
		if(p == nullptr) {
			FAIL("Couldn't load asset: '" << _bmp_path << "'");
			return;
		}
		auto tex = SDL_CreateTextureFromSurface(ren, p);
		bmp.emplace_back(p,tex);
		DEBUG("Loaded '" << _bmp_path << "'");
	}
	ready = true;
}

std::pair<std::size_t,std::size_t> Actor::load_bmp_assets(const char* _bmp_path, std::size_t _bmp_count, std::size_t increment) {
	std::array<char,512> buf;
	std::size_t ok = 0, bad =0;
	for(std::size_t i=0; i < _bmp_count; i += increment) {
		std::fill(buf.begin(),buf.end(),0);
		int r = snprintf(&buf[0],sizeof(buf)-1,_bmp_path,i);
		if(r < 0) {
			FAIL("Couldn't build string using snprintf for _bmp_path: '" << _bmp_path << "' on the " << i << "th iteration");
			++bad;
			continue;
		}
		auto p = SDL_LoadBMP(&buf[0]);
		if(p == nullptr) {
			FAIL("Couldn't load asset: '" << &buf[0] << "'");
			++bad;
			continue;
		}
		auto tex = SDL_CreateTextureFromSurface(ren, p);
		if(tex == nullptr) {
			SDL_FreeSurface(p);
			++bad;
			continue;
		}
		bmp.emplace_back(p,tex);
		++ok;
		DEBUG("Loaded '" << &buf[0] << "'");
	}
	return {ok,bad};
}
bool Actor::load_bmp_asset(const char* _bmp_path) {
	auto p = SDL_LoadBMP(_bmp_path);
	if(p == nullptr) {
		FAIL("Couldn't load asset: '" << _bmp_path << "'");
		return false;
	}
	auto tex = SDL_CreateTextureFromSurface(ren, p);
	if(tex == nullptr) {
		SDL_FreeSurface(p);
		return false;
	}
	bmp.emplace_back(p,tex);
	DEBUG("Loaded '" << _bmp_path << "'");
	return true;
}
std::pair<std::size_t,std::size_t> Actor::load_bmp_assets(const char* _bmp_path, std::size_t _bmp_count) {
	if(_bmp_count) {
		std::array<char,512> buf;
		std::size_t ok = 0, bad =0;
		for(std::size_t i=0; i < _bmp_count; ++i) {
			std::fill(buf.begin(),buf.end(),0);
			int r = snprintf(&buf[0],sizeof(buf)-1,_bmp_path,i);
			if(r < 0) {
				FAIL("Couldn't build string using snprintf for _bmp_path: '" << _bmp_path << "' on the " << i << "th iteration");
				++bad;
				continue;
			}
			auto p = SDL_LoadBMP(&buf[0]);
			if(p == nullptr) {
				FAIL("Couldn't load asset: '" << &buf[0] << "'");
				++bad;
				continue;
			}
			auto tex = SDL_CreateTextureFromSurface(ren, p);
			if(tex == nullptr) {
				SDL_FreeSurface(p);
				++bad;
				continue;
			}
			bmp.emplace_back(p,tex);
			++ok;
			DEBUG("Loaded '" << &buf[0] << "'");
		}
		return {ok,bad};
	}
	auto p = SDL_LoadBMP(_bmp_path);
	if(p == nullptr) {
		FAIL("Couldn't load asset: '" << _bmp_path << "'");
		return {0,1};
	}
	auto tex = SDL_CreateTextureFromSurface(ren, p);
	if(tex == nullptr) {
		return {0,2};
	}
	bmp.emplace_back(p,tex);
	DEBUG("Loaded '" << _bmp_path << "'");
	return {1,0};
}
Actor::~Actor() {
	if(ready) {
		for(const auto& b : bmp) {
			if(b.surface) {
				SDL_FreeSurface(b.surface);
			}
			if(b.texture) {
				SDL_DestroyTexture(b.texture);
			}
		}
	}
}
void Actor::calc() {
	this->cx = this->x + this->rect.w / 2;
	this->cy = this->y + this->rect.h / 2;
}

/** Copy constructor */
Actor::Actor(const Actor& other) {
	std::cout << "Actor::copy constructor\n";
	this->x = other.x;
	this->y = other.y;
	this->rect = other.rect;
	this->calc();// must be called after rect is assigned ALWAYS
	this->bmp = other.bmp;
	this->ready = other.ready;
}
std::string Actor::report() const {
	std::string s;
	s = "x: ";
	s += std::to_string(x);
	s += "y: ";
	s += std::to_string(y);
	s += "r.x: ";
	s += std::to_string(rect.x);
	s += "r.y: ";
	s += std::to_string(rect.y);
	s += "r.w: ";
	s += std::to_string(rect.w);
	s += "r.h: ";
	s += std::to_string(rect.h);
	return s;
}
