#include "DirectX.h"

#define BYTE_TO_FLOAT(b) (b / 255.f)
#define TO_BOUNDINGBOX(x, y, width, height) \
	{ (float)x, (float)y, (float)x + width, (float)y + height }
#define TO_POINT(x, y) \
	{ (float)x, (float)y }
#define TO_RGBFLOAT(color) \
	{ BYTE_TO_FLOAT(color.r), BYTE_TO_FLOAT(color.g), BYTE_TO_FLOAT(color.b), BYTE_TO_FLOAT(color.a) }

Renderer* DirectX::CreateMainTarget(HWND window) {
	if(mainInst) return mainInst;

	if(FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		throw runtime_error("failed to create directx instance");
	if(FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pDWriteFactory),
								  reinterpret_cast<IUnknown**>(&pDWriteFactory))))
		throw runtime_error("failed to create dwrite instance");

	RECT rc;
	GetClientRect(window, &rc);

	DirectX::window = window;

	ID2D1HwndRenderTarget* target;
	IDWriteTextFormat* textFormat;
	ID2D1SolidColorBrush* brush;

	HRESULT hr = pFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(window, {(UINT32)rc.right, (UINT32)rc.bottom}, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		&target);


	if(SUCCEEDED(hr))
		hr = pDWriteFactory->CreateTextFormat(L"Verdana", // clean ui font imo
											  NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
											  DWRITE_FONT_STRETCH_NORMAL, 12,
											  L"", // locale
											  &textFormat);


	if(SUCCEEDED(hr)) hr = target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);

	textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	mainInst = new DirectX();
	mainInst->pRenderTarget = target;
	mainInst->pBrush = brush;
	mainInst->pTextFormat = textFormat;

	return mainInst;
}

void DirectX::ResizeMainRenderer() {
	RECT rc;
	GetClientRect(window, &rc);
	((ID2D1HwndRenderTarget*)mainInst->pRenderTarget)->Resize({(UINT32)rc.right, (UINT32)rc.bottom});
}

void DirectX::Resize() {
	DirectX::ResizeMainRenderer();
}

DirectX::~DirectX() {
	pRenderTarget->Release();
	pBrush->Release();
	pTextFormat->Release();
}

Color color = {0};
inline ID2D1SolidColorBrush* SetColor(ID2D1SolidColorBrush* b, const Color& c) {
	if(!(color == c)) {
		color = c;
		b->SetColor(TO_RGBFLOAT(c));
	}
	return b;
}

void DirectX::DrawLine(int x0, int y0, int x1, int y1, const Color& color, float w) {
	pRenderTarget->DrawLine(TO_POINT(x0, y0), TO_POINT(x1, y1), SetColor(pBrush, color), w);
}

void DirectX::DrawRoundedRect(float x, float y, float width, float height, const Color& color, float radius) {
	radius = ((height / 2) * radius);
	pRenderTarget->DrawRoundedRectangle({TO_BOUNDINGBOX(x, y, width, height), radius, radius}, SetColor(pBrush, color));
}

void DirectX::DrawRect(float x, float y, float width, float height, const Color& color) {
	pRenderTarget->DrawRectangle(TO_BOUNDINGBOX(x, y, width, height), SetColor(pBrush, color));
}

void DirectX::DrawString(int x, int y, int width, int height, const Color& color, string text) {
	const size_t cSize = text.length() + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, text.c_str(), cSize);
	pRenderTarget->DrawTextA(wc, cSize, pTextFormat, TO_BOUNDINGBOX(x, y, width, height), SetColor(pBrush, color));
	delete[] wc;
}

void DirectX::FillRoundedRect(float x, float y, float width, float height, const Color& color, float radius) {
	radius = ((height / 2) * radius);
	pRenderTarget->FillRoundedRectangle({x, y, x + width, y + height, radius, radius}, SetColor(pBrush, color));
}

void DirectX::FillRect(float x, float y, float width, float height, const Color& color) {
	pRenderTarget->FillRectangle({x, y, x + width, y + height}, SetColor(pBrush, color));
}

void DirectX::BeginDraw() { pRenderTarget->BeginDraw(); }

bool DirectX::EndDraw() {
	HRESULT res = pRenderTarget->EndDraw();
	return res == D2DERR_RECREATE_TARGET ? false : SUCCEEDED(res);
}

Sprite* DirectX::LoadSpriteFromBitmap(const vector<uint8_t>& bytes) { return new DirectXSprite(this, 0,0); }
Sprite* DirectX::LoadSpriteFromFile(string filePath) { return new DirectXSprite(this, 0,0); }
Sprite* DirectX::CreateSprite(int width, int height) { return new DirectXSprite(this, width, height); }

void DirectX::Clear(const Color& color) {
	pRenderTarget->Clear({BYTE_TO_FLOAT(color.r), BYTE_TO_FLOAT(color.g), BYTE_TO_FLOAT(color.b), BYTE_TO_FLOAT(color.a)});
}

void DirectX::SetTextAlign(TextAlignment a) {
	switch(a) {
	case TextAlignment::LEFT:
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case TextAlignment::RIGHT:
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	case TextAlignment::CENTER:
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	}
}

void DirectX::DrawSprite(float x, float y, float width, float height, Sprite* texture, bool linearScale, float alpha) {
	pRenderTarget->DrawBitmap(((DirectXSprite*)texture)->bitmap, TO_BOUNDINGBOX(x, y, width, height), alpha,
							  (D2D1_BITMAP_INTERPOLATION_MODE)linearScale);
}