#ifndef __LINE_HEADER__
#define __LINE_HEADER__
#include <iostream>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <vector>
struct Point {
	double x;
	double y;
};

struct Line {
	Point p1, p2;
	Line() = default;
	~Line() = default;

	Line(Point p1, Point p2) {
		p1 = p1;
		p2 = p2;
	}
	Line(const Line& o) {
		p1 = o.p1;
		p2 = o.p2;
	}
	std::vector<Point> points;

	const std::vector<Point>& getPoints(std::size_t quantity) {
		points.clear();
		int ydiff = p2.y - p1.y, xdiff = p2.x - p1.x;
		double slope = (double)(p2.y - p1.y) / (p2.x - p1.x);
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
	int angle;
};

struct floatPoint {
	float x;
	float y;
};

#endif
