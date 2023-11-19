#pragma once
// integer Vector2D
struct iV2D {
	int x, y;
};
// float Vector2D
struct fV2D {
	float x, y;
};

struct fV4D {
	float a, b, c, d;
};

struct Color {
	uint8_t r, g, b, a;
	Color(unsigned rgb, uint8_t a = 255) : r((rgb & 0xFF0000) >> 16), g((rgb & 0x00FF00) >> 8), b(rgb & 0x0000FF), a(a) {}
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a){};
	inline bool operator==(const Color& c) { return c.a == a & c.r == r & c.g == g & c.b == b; }
	inline static Color random() { return Color(rand() % 0xff, rand() % 0xff, rand() % 0xff); }
	inline operator fV4D() const { return {r / 255.f, g / 255.f, b / 255.f, a / 255.f}; }
};

class Colors {
  public:
	static inline Color white = Color(0xffffff);
	static inline Color black = Color(0);
};