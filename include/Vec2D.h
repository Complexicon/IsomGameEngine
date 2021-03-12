#ifndef VEC_H_
#define VEC_H_

#include <math.h>

struct Vec2D {
	float x, y;
	Vec2D(float x, float y) : x(x), y(y) {}
	void invertX() { x *= -1; }
	void invertY() { y *= -1; }
	void reverse() {
		invertX();
		invertY();
	}
	Vec2D operator+(Vec2D add) { return Vec2D(x + add.x, y + add.y); }
	Vec2D operator-(Vec2D sub) { return Vec2D(x - sub.x, y - sub.y); }
	Vec2D operator*(float mul) { return Vec2D(x * mul, y * mul); }
	Vec2D operator/(float div) { return Vec2D(x / div, y / div); }
	float len() { return sqrt(x * x + y * y); }
	float dist(Vec2D& other) { return operator-(other).len(); }
	Vec2D getRebounced(Vec2D n) {
		Vec2D v = *this;
		return v - (n * ((n.x * v.x + n.y * v.y) / (n.len() * n.len()))) * 2;
	}
};

#endif