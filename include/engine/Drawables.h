#pragma once
#include "pch.h"
#include "Font.h"

struct Texture;

struct LineTo {
	fV2D point;
	Color color = Colors::white;
};

struct Rectangle {
	fV2D size;
	Color color = Colors::white;
	bool fill = true;
	float borderRadius = 0;
};

struct Sprite {
	Texture* texture;
	Color tint = Colors::white;
	fV2D size;
};

struct Text2D {
	string text;
	Color color = Colors::white;
	Font* font;
};