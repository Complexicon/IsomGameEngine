#ifndef SPRITE_H
#define SPRITE_H

#include <d2d1.h>

class Renderer;

class Sprite{
	ID2D1Bitmap* bitmap;
	public:
		Sprite(const char* filename, ID2D1RenderTarget* renderer);
		~Sprite();

	friend class Renderer;
};

#endif