#pragma once

#include "pch.h"

struct Glyph {
	iV2D texCoords;
	iV2D size;
	iV2D bearing;
	uint32_t advance;
};

struct Font {

	iV2D size;
	int ascender;
	int descender;
	int lineGap;
	vector<uint8_t> bitmap;
	map<char, Glyph> glyphs;

	Font(string filePath, float size);
};