#undef UNICODE
#include <d2d1.h>
#include <dwrite.h>

#define BYTE_TO_FLOAT(b) (b / 255.f)
#define TO_BOUNDINGBOX(x, y, width, height) \
	{ (float)x, (float)y, (float)x + width, (float)y + height }
#define TO_POINT(x, y) \
	{ (float)x, (float)y }
#define TO_RGBFLOAT(color) \
	{ BYTE_TO_FLOAT(color.r), BYTE_TO_FLOAT(color.g), BYTE_TO_FLOAT(color.b), BYTE_TO_FLOAT(color.a) }

typedef const char* str;

struct Color {
	byte r, g, b, a;
	Color(unsigned rgb, byte a = 255);
	Color(byte r, byte g, byte b, byte a = 255);
	inline bool operator==(const Color& c) { return c.a == a & c.r == r & c.g == g & c.b == b; }
	inline static Color random() { return Color(rand() % 0xff, rand() % 0xff, rand() % 0xff); }
};

enum class TextAlignment {
	LEFT,
	RIGHT,
	CENTER,
};

class Renderer {

  public:
  private:
	ID2D1Factory* pFactory = 0;
	ID2D1HwndRenderTarget* pRenderTarget = 0;
	ID2D1SolidColorBrush* pBrush = 0;
	IDWriteTextFormat* pTextFormat = 0;
	IDWriteFactory* pDWriteFactory = 0;
	HWND window;
	int w, h;

  public:
	Renderer(HWND window);
	void DrawLine(int x0, int y0, int x1, int y1, const Color& color, float w = 1.f);
	void DrawRoundedRect(float x, float y, float width, float height, const Color& color, float radius = 1.f);
	void DrawRect(float x, float y, float width, float height, const Color& color);
	void DrawString(int x, int y, int width, int height, const Color& color, str text);
	void FillRoundedRect(float x, float y, float width, float height, const Color& color, float radius = 1.f);
	void FillRect(float x, float y, float width, float height, const Color& color);
	void Clear(const Color& color);
	void SetTextAlign(TextAlignment a);
	int GetWidth() { return w; }
	int GetHeight() { return h; }

  private:
	bool Setup();
	void BeginDraw();
	bool EndDraw();
	bool CreateTarget();
	void DiscardTarget();
	void Destroy();
	void Resize();

	friend class Game;
};