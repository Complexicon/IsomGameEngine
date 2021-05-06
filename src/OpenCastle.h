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
		game->gameTPS = 150;
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

		game->additionalDebugInfo = (
			VECPOS("cam", tmp) + 
			VECPOS("mouse", mouse) + 
			VECPOS("mouse selected tile", selectedTile) +
			line("drawing " + draw)
		);

	}

	void userinput(const UserInputEvent& e){
		switch(e.type){
			case UserInputType::Mouse:
				if(CAST(MouseInputEvent, e).mouseEvent == MouseEventType::Move){
					MouseMoveEvent me = CAST(MouseMoveEvent, e);
					mouse = { FLOATIFY(me.mouseX, w), FLOATIFY(me.mouseY, h) };
					mouseReal = { me.mouseX, me.mouseY };
				}else{
					MouseClickEvent me = CAST(MouseClickEvent, e);
					if(me.button != MouseButton::Left) return;
					draw = me.pressed;
				}
				return;
			case UserInputType::Keyboard:
				KeyboardInputEvent ke = CAST(KeyboardInputEvent, e);
				if(!ke.pressed) return;
				switch(ke.key) {
				case Key::ADD: zoom(true); return;
				case Key::SUBTRACT: zoom(); return;
				case Key::LBUTTON: draw = !draw; return;
				case Key::CONTROL: game->drawDebug = !game->drawDebug; return;
				case Key::ESCAPE: game->activeScene = pause; return;
				}
				return;
		}
	}
};

#endif