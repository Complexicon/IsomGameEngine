#include "Renderer.h"

Renderer::Renderer(HWND window) : window(window) { Setup(); }

void Renderer::DrawLine(int x0, int y0, int x1, int y1, const Color& color, float w) {
	pBrush->SetColor(TO_RGBFLOAT(color));
	pRenderTarget->DrawLine(TO_POINT(x0, y0), TO_POINT(x1, y1), pBrush, w);
}

void Renderer::DrawRoundedRect(float x, float y, float width, float height, const Color& color, float radius) {
	pBrush->SetColor(TO_RGBFLOAT(color));
	radius = ((height / 2) * radius);
	pRenderTarget->DrawRoundedRectangle({TO_BOUNDINGBOX(x, y, width, height), radius, radius}, pBrush);
}

void Renderer::DrawString(int x, int y, int width, int height, const Color& color, str text) {
	pBrush->SetColor(TO_RGBFLOAT(color));
	const size_t cSize = strlen(text) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, text, cSize);
	pRenderTarget->DrawTextA(wc, cSize, pTextFormat, TO_BOUNDINGBOX(x, y, width, height), pBrush);
	delete[] wc;
}

void Renderer::DrawPixel(int x, int y, const Color& color) {
	pBrush->SetColor(TO_RGBFLOAT(color));
	pRenderTarget->FillRectangle(TO_BOUNDINGBOX(x, y, x, y), pBrush);
}

void Renderer::FillRoundedRect(float x, float y, float width, float height, const Color& color, float radius) {
	pBrush->SetColor(TO_RGBFLOAT(color));
	radius = ((height / 2) * radius);
	pRenderTarget->FillRoundedRectangle({TO_BOUNDINGBOX(x, y, width, height), radius, radius}, pBrush);
}

bool Renderer::Setup() {
	if(FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) return false;
	if(FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(pDWriteFactory),
								  reinterpret_cast<IUnknown**>(&pDWriteFactory))))
		return false;
	return true;
}

bool Renderer::CreateTarget() {
	HRESULT hr = S_OK;
	if(pRenderTarget == NULL) {
		RECT rc;
		GetClientRect(window, &rc);
		w = rc.right;
		h = rc.bottom;

		// Create all graphic resources

		hr = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
											  D2D1::HwndRenderTargetProperties(window, {(UINT32)rc.right, (UINT32)rc.bottom}),
											  &pRenderTarget);

		// TODO: relocate directwrite

		if(SUCCEEDED(hr))
			hr = pDWriteFactory->CreateTextFormat(L"Verdana", // clean ui font imo
												  NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
												  DWRITE_FONT_STRETCH_NORMAL, 12,
												  L"", // locale
												  &pTextFormat);

		if(SUCCEEDED(hr)) hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);

		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	return SUCCEEDED(hr);
}

void Renderer::DiscardTarget() {
	// release d2d
	pRenderTarget->Release();
	pBrush->Release();
	pRenderTarget = 0;
	pBrush = 0;
}

void Renderer::Destroy() {
	pDWriteFactory->Release();
	pFactory->Release();
}

void Renderer::BeginDraw() { pRenderTarget->BeginDraw(); }

bool Renderer::EndDraw() {
	HRESULT res = pRenderTarget->EndDraw();
	return res == D2DERR_RECREATE_TARGET ? false : SUCCEEDED(res);
}

void Renderer::Resize() {
	if(!CreateTarget()) return;
	RECT rc;
	GetClientRect(window, &rc);
	pRenderTarget->Resize({(UINT32)rc.right, (UINT32)rc.bottom});
}

void Renderer::Clear(const Color& color) {
	pRenderTarget->Clear({BYTE_TO_FLOAT(color.r), BYTE_TO_FLOAT(color.g), BYTE_TO_FLOAT(color.b), BYTE_TO_FLOAT(color.a)});
}

void Renderer::SetTextAlign(TextAlignment a) {
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

Color::Color(unsigned rgb, byte a) : r((rgb & 0xFF0000) >> 16), g((rgb & 0x00FF00) >> 8), b(rgb & 0x0000FF), a(a) {}

Color::Color(byte r, byte g, byte b, byte a) : r(r), g(g), b(b), a(a) {}
