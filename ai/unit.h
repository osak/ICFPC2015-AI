#pragma once
#include "common.h"

using namespace std;

class Point {
public:

	int x;
	int y;

	bool operator<(const Point &p) const {
		if (x != p.x) return x < p.x;
		return y < p.y;
	}
};

class Unit {
public:

	Point pivot;
	vector <Point> member;
};

