#pragma once

#include "pch.h"



enum class TextAlignment {
	LEFT,
	RIGHT,
	CENTER,
};

class Sprite;

class Renderer {
  public:
	virtual ~Renderer(){};
	virtual void DrawLine(int x0, int y0, int x1, int y1, const Color& color, float w = 1.f) = 0;
	virtual void DrawSprite(float x, float y, float width, float height, Sprite* texture, bool linearScale = true, float alpha = 1.f) = 0;
	virtual void DrawRoundedRect(float x, float y, float width, float height, const Color& color, float radius = 1.f) = 0;
	virtual void DrawRect(float x, float y, float width, float height, const Color& color) = 0;
	virtual void DrawString(int x, int y, int width, int height, const Color& color, string text) = 0;
	virtual void FillRoundedRect(float x, float y, float width, float height, const Color& color, float radius = 1.f) = 0;
	virtual void FillRect(float x, float y, float width, float height, const Color& color) = 0;
	virtual void Clear(const Color& color) = 0;
	virtual void SetTextAlign(TextAlignment a) = 0;
	virtual Sprite* CreateSprite(int width, int height) = 0;
	virtual Sprite* LoadSpriteFromFile(string filePath) = 0;
	virtual Sprite* LoadSpriteFromBitmap(const vector<uint8_t>& bytes) = 0;
	virtual void BeginDraw() = 0;
	virtual bool EndDraw() = 0;
  private:
	virtual void Resize() = 0;

	friend class Game;
};

class Sprite {
	public:
		virtual ~Sprite(){};
		virtual Renderer* target() = 0;
};