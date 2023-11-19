#pragma once
#include "engine/Platform.h"
#include "pch.h"

#undef UNICODE
#include "windows.h"
#include <d2d1.h>
#include <dwrite.h>

class DirectX : public Renderer {

  public:
	static Renderer* CreateMainTarget(HWND handle);
	static void ResizeMainRenderer();
	static inline HWND window;
	static inline IDWriteFactory* pDWriteFactory = 0;
	static inline ID2D1Factory* pFactory = 0;
	static inline DirectX* mainInst = 0;

  private:
	IDWriteTextFormat* pTextFormat = 0;
	ID2D1SolidColorBrush* pBrush = 0;
	ID2D1RenderTarget* pRenderTarget;

  private:
	void BeginDraw();
	bool EndDraw();
	void Resize();

  public:
	void DrawLine(int x0, int y0, int x1, int y1, const Color& color, float w = 1.f);
	void DrawSprite(float x, float y, float width, float height, Sprite* texture, bool linearScale = true, float alpha = 1.f);
	void DrawRoundedRect(float x, float y, float width, float height, const Color& color, float radius = 1.f);
	void DrawRect(float x, float y, float width, float height, const Color& color);
	void DrawString(int x, int y, int width, int height, const Color& color, string text);
	void FillRoundedRect(float x, float y, float width, float height, const Color& color, float radius = 1.f);
	void FillRect(float x, float y, float width, float height, const Color& color);
	void Clear(const Color& color);
	void SetTextAlign(TextAlignment a);

	Sprite* CreateSprite(int width, int height);
	Sprite* LoadSpriteFromFile(string filePath);
	Sprite* LoadSpriteFromBitmap(const vector<uint8_t>& bytes);

	~DirectX();

	friend class DirectXSprite;
};

class DirectXSprite : public Sprite {
  public:

	ID2D1BitmapRenderTarget* renderTarget;
	ID2D1Bitmap* bitmap;
	DirectX* instance;

	~DirectXSprite() {
		bitmap->Release();
		delete instance;
	}

	DirectXSprite(DirectX* dx, int width, int height) {
		D2D1_SIZE_U size = {(uint32_t)width, (uint32_t)height};
		D2D1_SIZE_F size2 = {(float)width, (float)height};
		HRESULT hr = dx->pRenderTarget->CreateCompatibleRenderTarget(size2, size, &renderTarget);
		renderTarget->GetBitmap(&bitmap);

		IDWriteTextFormat* textFormat;
		ID2D1SolidColorBrush* brush;

		hr = DirectX::pDWriteFactory->CreateTextFormat(L"Verdana", // clean ui font imo
											  NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
											  DWRITE_FONT_STRETCH_NORMAL, 12,
											  L"", // locale
											  &textFormat);

		if(SUCCEEDED(hr)) hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);

		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		instance = new DirectX();
		instance->pRenderTarget = renderTarget;
		instance->pBrush = brush;
		instance->pTextFormat = textFormat;
	}

	Renderer* target() {
		return instance;
	}

	// DirectXSprite(DirectX* dx, int width, int height) {

	// }

	// DirectXSprite(DirectX* dx, int width, int height) {

	// }
};