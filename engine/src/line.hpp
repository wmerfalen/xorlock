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

	Line(Point p1, Point p2);
	Line(const Line& o);
	std::vector<Point> points;

	const std::vector<Point>& getPoints(std::size_t quantity);
	int angle;
};

struct floatPoint {
	float x;
	float y;
};


#endif
