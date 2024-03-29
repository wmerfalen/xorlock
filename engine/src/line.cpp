#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>
#include "line.hpp"
#include "world.hpp"

Line::Line(Point _p1, Point _p2) {
	p1 = _p1;
	p2 = _p2;
  registered_with_movement_system = false;
}
Line::Line(const Line& o) {
	p1 = o.p1;
	p2 = o.p2;
	angle = o.angle;
  registered_with_movement_system = false;
}
const std::vector<Point>& Line::getPoints(std::size_t quantity) {
	points.clear();
	const int ydiff = p2.y - p1.y, xdiff = p2.x - p1.x;
	const double slope = (double)(p2.y - p1.y) / (p2.x - p1.x);
	double x, y;

	--quantity;

	for(double i = 0; i < quantity; i++) {
		y = slope == 0 ? 0 : ydiff * (i / quantity);
		x = slope == 0 ? xdiff * (i / quantity) : y / slope;
		points.emplace_back(x + p1.x, y + p1.y);
	}

	points[quantity] = p2;
	return points;
}
void Line::register_with_movement_system(){
  for(auto& p : points){
    register_xy(&p.x,&p.y);
  }
  registered_with_movement_system = true;
}
void Line::unregister_with_movement_system(){
  for(auto& p : points){
    unregister_xy(&p.x,&p.y);
  }
  registered_with_movement_system = false;
}
