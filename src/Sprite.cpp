#include "Sprite.h"
#include <wincodec.h>

Sprite::Sprite(const char* filename, ID2D1RenderTarget* r){
	HRESULT hr;
	IWICImagingFactory* pFactory = 0;
	
	CoInitializeEx(0,0);
	hr = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
	if(FAILED(hr)) return;

	const size_t cSize = strlen(filename) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, filename, cSize);

	IWICBitmapDecoder* decoder;
	hr = pFactory->CreateDecoderFromFilename(wc, 0, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
	delete[] wc;
	if(FAILED(hr)) return;

	IWICBitmapFrameDecode* frame;
	hr = decoder->GetFrame(0, &frame);
	if(FAILED(hr)) return;

	IWICFormatConverter* conv;
	hr = pFactory->CreateFormatConverter(&conv);
	if(FAILED(hr)) return;

	hr = conv->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, 0, 0, WICBitmapPaletteTypeMedianCut);
	if(FAILED(hr)) return;

	r->CreateBitmapFromWicBitmap(conv,0, &bitmap);

	conv->Release();
	frame->Release();
	decoder->Release();
	pFactory->Release();

	CoUninitialize();

}

Sprite::~Sprite(){
	if(bitmap) bitmap->Release();
}