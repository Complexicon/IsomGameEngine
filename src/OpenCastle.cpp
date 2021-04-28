#include "Game.h"
#include "Map.h"
#include "string/inc.h"
#include <windows.h>

#define line(x) (cmplx::String() + x + "\n")

#define IN_BOUNDS(vec, r, szX, szY) (vec.x - r >= 0 && vec.x + r <= szX && vec.y - r >= 0 && vec.y + r <= szY)
#define CEILDIV(x,y) ((x+y-1) / y)
#define FLOATIFY(in,max) ((((float)in / max) * 2) - 1.f)
#define VECPOS(name,vec) line(name + ": [x: " + vec.x + ", y: " + vec.y + "]")
#define PIXOFF(in) ((in - (int)in) * fieldSize)

void debug(Game* game, Str additonal = "") {
	Str defaultDebug = line("fps: " + game->getFPS());
	defaultDebug += line("tps: " + game->getTPS());
	defaultDebug += line("frametime: " + game->getFrameTime() * 1000 + "ms");
	defaultDebug += line("tickTime: " + game->getTickTime() * 1000 + "ms");
	defaultDebug += additonal;
	game->r->FillRect(0, 0, 250, 150, Color(0, 100));
	game->r->DrawString(0, 0, 250, 150, Color(0xffffff), defaultDebug.c_str());
}

class TestScene : public IScene {

	Map m;
	int fieldSize;
	int fieldsOnScreenX;
	int fieldsOnScreenY;
	fV2D cam;

	fV2D mouse;
	int w, h;
	iV2D mouseReal;
	iV2D selectedTile;
	bool draw = false;

	void init() {
		w = r->GetWidth();
		h = r->GetHeight();
		game->gameTPS = 128;
		fieldSize = 55;
		zoom();

#include "mapout.h"
		m.LoadMap(map, SZ_MAP);
	}

	void update(float delta) {
		if(mouse.x > 0.5 || mouse.x < -0.5) cam.x += mouse.x / 2;
		if(mouse.y > 0.5 || mouse.y < -0.5) cam.y += mouse.y / 2;
		selectedTile = pixelToTile(mouseReal);
		if(draw) m.SetTile(selectedTile.x, selectedTile.y, 0);
	}

	iV2D pixelToTile(iV2D in) { return {(in.x + (int)PIXOFF(cam.x)) / fieldSize + (int)cam.x, (in.y + (int)PIXOFF(cam.y)) / fieldSize + (int)cam.y}; }

	void zoom(bool in = false) {
		if(!in && fieldSize <= 5) return; // temp smallest cap
		fieldSize += in ? 5 : -5;
		fieldsOnScreenX = CEILDIV(w, fieldSize);
		fieldsOnScreenY = CEILDIV(h, fieldSize);
	}

	const Color tiles[4] = {Color(0x40eb34), Color(0x1ea3eb), Color(0x7d4e02), Color(0xffffff, 50)};

	void drawTile(int x, int y, unsigned tileData) {
		if(tileData > 3) return;
		r->FillRect(x * fieldSize - PIXOFF(cam.x), y * fieldSize - PIXOFF(cam.y), fieldSize + 1, fieldSize + 1, tiles[tileData]);
	}

	void render() {
		r->Clear(Color(0x111111));

		for(int y = 0; y < fieldsOnScreenY; y++) {
			for(int x = 0; x < fieldsOnScreenX; x++) { drawTile(x, y, m.Get(((int)cam.x + x), ((int)cam.y + y))); }
		}

		drawTile(selectedTile.x - (int)cam.x, selectedTile.y - (int)cam.y, 3);
		// debug overlay
		debug(game, 
			VECPOS("cam", cam) + 
			VECPOS("mouse", mouse) + 
			VECPOS("mouse selected tile", selectedTile) +
			line("drawing " + draw)
		);
	}

	void userinput(bool mouseEvent, int param1, int param2) {
		if(mouseEvent) {
			mouse = { FLOATIFY(param1, w), FLOATIFY(param2, h) };
			mouseReal = { param1, param2 };
			return;
		}
		switch(param1) {
		case VK_ADD: zoom(true); return;
		case VK_SUBTRACT: zoom(); return;
		case VK_LBUTTON: draw = !draw; return;
		case 0x57: cam.y -= 0.5f; return;
		case 0x53: cam.y += 0.5f; return;
		case 0x41: cam.x -= 0.5f; return;
		case 0x44: cam.x += 0.5f; return;
		case VK_ESCAPE: PostQuitMessage(0); return;
		}
	}
};

int main() {
	Game(new TestScene(), "Testgame").start();
	return 1;
}
