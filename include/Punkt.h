#ifndef PNKT_H_
#define PNKT_H_

#include "Vec2D.h"
struct Color;
class Punkt {
  public:
	float r;
	Vec2D vec, pos;
	Color c = {0};

	Punkt(Vec2D pos) : vec(0, 0), pos(0, 0), r((rand() % 20) + 10) {
		this->pos = pos;
		c = Color::random();
		vec = Vec2D(float(rand() % 200) / 100 - 1.0, float(rand() % 200) / 100 - 1.0);
	}
};

#endif
