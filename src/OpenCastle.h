#ifndef OPNCSTL
#define OPNCSTL

#include "Game.h"
#include "Map.h"
#include "string/inc.h"
#include <windows.h>
#include "utilities.h"

#include "Pause.h"

#define line(x) (cmplx::String() + x + "\n")
#define VECPOS(name,vec) line(name + ": [x: " + vec.x + ", y: " + vec.y + "]")

#define PIXOFF(in) ((in - (int)in) * fieldSize)
#define PIXTOTILE(in) {(in.x + (int)PIXOFF(cam.x)) / fieldSize + (int)cam.x, (in.y + (int)PIXOFF(cam.y)) / fieldSize + (int)cam.y}

#define IN_BOUNDS(vec, r, szX, szY) (vec.x - r >= 0 && vec.x + r <= szX && vec.y - r >= 0 && vec.y + r <= szY)
#define CEILDIV(x,y) ((x+y-1) / y)
#define FLOATIFY(in,max) ((((float)in / max) * 2) - 1.f)

inline void debug(Game* game, Str additonal = "") {
	Str defaultDebug = line("fps: " + game->getFPS());
	defaultDebug += line("tps: " + game->getTPS());
	defaultDebug += line("frametime: " + game->getFrameTime() * 1000 + "ms");
	defaultDebug += line("tickTime: " + game->getTickTime() * 1000 + "ms");
	defaultDebug += additonal;
	game->r->FillRect(0, 0, 250, 150, Color(0, 100));
	game->r->DrawString(0, 0, 250, 150, Color(0xffffff), defaultDebug.c_str());
}

class OpenCastle : public IScene {

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

	Sprite* tiles[3];

	IScene* pause;

	void init() {
		w = r->GetWidth();
		h = r->GetHeight();
		fieldSize = 55;
		game->gameTPS = 128;
		game->targetFps = 200;
		zoom();
		tiles[0] = r->CreateTexture("textures/grass.png");
		tiles[2] = r->CreateTexture("textures/dirt.png");
		tiles[1] = r->CreateTexture("textures/water.png");
#include "mapout.h"
		m.LoadMap(map, SZ_MAP);

		// setup scenes here
		pause = game->InitScene(new PauseMenu(this));
	}

	void update(float delta) {
		if(mouse.x > 0.8 || mouse.x < -0.8) cam.x += mouse.x / 2;
		if(mouse.y > 0.7 || mouse.y < -0.7) cam.y += mouse.y / 2;
		selectedTile = PIXTOTILE(mouseReal);
		if(draw) m.SetTile(selectedTile.x, selectedTile.y, 0);
	}

	void zoom(bool in = false) {
		if(!in && fieldSize <= 10) return; // temp smallest cap
		fieldSize += in ? 5 : -5;
		fieldsOnScreenX = CEILDIV(w, fieldSize);
		fieldsOnScreenY = CEILDIV(h, fieldSize);
	}

	void drawTile(int x, int y, unsigned tileData, fV2D tempCam, fV2D pixelOff) {
		if(tileData > 3 || tileData < 0) return;
		switch(tileData){
			case 3:
				r->FillRect(x * fieldSize - pixelOff.x, y * fieldSize - pixelOff.y, fieldSize + 1, fieldSize + 1, Color(0xffffff, 50));
				break;
			default:
				r->DrawSprite(x * fieldSize - pixelOff.x, y * fieldSize - pixelOff.y, fieldSize + 1, fieldSize + 1, tiles[tileData], false);
				break;
		}
	}

	void render() {
		r->Clear(Color(0x111111));

		fV2D tmp = cam; // tearing fix
		fV2D pxOff = { PIXOFF(tmp.x), PIXOFF(tmp.y)};

		for(int y = 0; y < fieldsOnScreenY + 1; y++) {
			for(int x = 0; x < fieldsOnScreenX + 1; x++) { drawTile(x, y, m.Get(((int)tmp.x + x), ((int)tmp.y + y)), tmp, pxOff); }
		}

		drawTile(selectedTile.x - (int)tmp.x, selectedTile.y - (int)tmp.y, 3, tmp, pxOff);
		// debug overlay
		debug(game, 
			VECPOS("cam", tmp) + 
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
		case VK_ESCAPE: game->activeScene = pause; return;
		}
	}
};

#endif