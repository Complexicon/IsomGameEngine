#pragma once

#include "pch.h"
#include "Drawables.h"


class Renderer {
  public:
	uint32_t layer = 0;

	virtual ~Renderer(){};

	virtual void Clear(const Color& color = 0) = 0;

	virtual void DrawAt(float x, float y, const struct LineTo& line) = 0;
	virtual void DrawAt(float x, float y, const struct Rectangle& rect) = 0;
	virtual void DrawAt(float x, float y, const struct Sprite& sprite) = 0;
	virtual void DrawAt(float x, float y, const struct Text2D& text) = 0;

	virtual Texture* LoadSpriteFromFile(string filePath) = 0;
	virtual Texture* LoadSpriteFromBitmap(const vector<uint8_t>& bytes) = 0;

	virtual void BeginDraw() = 0;
	virtual bool EndDraw() = 0;
};