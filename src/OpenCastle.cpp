#include "Game.h"
#include "Map.h"
#include "string/inc.h"
#include <windows.h>

#define line(x) (cmplx::String() + x + "\n")

#define IN_BOUNDS(vec, r, szX, szY) (vec.x - r >= 0 && vec.x + r <= szX && vec.y - r >= 0 && vec.y + r <= szY)

void debug(Game* game, Str additonal = "") {
	Str defaultDebug = line("fps: " + game->getFPS());
	defaultDebug += line("tps: " + game->getTPS());
	defaultDebug += line("frametime: " + game->getFrameTime() * 1000 + "ms");
	defaultDebug += line("tickTime: " + game->getTickTime() * 1000 + "ms");
	defaultDebug += additonal;
	game->r->FillRoundedRect(0, 0, 200, 150, Color(0x0, 100), 0);
	game->r->DrawString(0, 0, 200, 150, Color(0xffffff), defaultDebug.c_str());
}

class Penis : public IScene {
	void render() {
		r->Clear(0);
		r->DrawString(0, 0, r->GetWidth(), r->GetHeight(), Color(0x554433), "Penis!");
	}
	void update(float delta) {}
	void userinput(bool mouseEvent, int param1, int param2) {}
};

class TestScene : public IScene {

	Map m;
	int fieldSize;
	int fieldsOnScreenX;
	int fieldsOnScreenY;
	float camX = 0, camY = 0;
	float mX = 0;
	float mY = 0;

	void init() {
		game->gameTPS = 128;
		fieldSize = 55;
		zoom();

#include "mapout.h"
		m.LoadMap(map, SZ_MAP);
	}

	void update(float delta) {
		int w = r->GetWidth();
		int h = r->GetHeight();
		if(mX > 0.5 || mX < -0.5) camX += mX / 2;
		if(mY > 0.5 || mY < -0.5) camY += mY / 2;
	}

	void zoom(bool in = false) {
		if(!in && fieldSize <= 5) return; // temp smallest cap
		fieldSize += in ? 5 : -5;
		fieldsOnScreenX = (r->GetWidth() + fieldSize - 1) / fieldSize;
		fieldsOnScreenY = (r->GetHeight() + fieldSize - 1) / fieldSize;
	}

	void render() {
		r->Clear(Color(0x111111));

		float rOX = (camX - (int)camX) * fieldSize;
		float rOY = (camY - (int)camY) * fieldSize;

		for(int y = 0; y < fieldsOnScreenY; y++) {
			for(int x = 0; x < fieldsOnScreenX; x++) {
				switch(m.Get(((int)camX + x), ((int)camY + y))) {
				case 0:
					r->FillRect(x * fieldSize - rOX, y * fieldSize - rOY, fieldSize + 1, fieldSize + 1, Color(0x40eb34));
					break;
				case 1:
					r->FillRect(x * fieldSize - rOX, y * fieldSize - rOY, fieldSize + 1, fieldSize + 1, Color(0x1ea3eb));
					break;
				case 2:
					r->FillRect(x * fieldSize - rOX, y * fieldSize - rOY, fieldSize + 1, fieldSize + 1, Color(0x7d4e02));
					break;
				}
			}
		}
		// debug overlay
		debug(game, line("camX: " + camX + " camY: " + camY) + line("mX: " + mX + " mY: " + mY));
	}

	void userinput(bool mouseEvent, int param1, int param2) {
		if(mouseEvent) {
			mX = (((float)param1 / r->GetWidth()) * 2) - 1.f;
			mY = (((float)param2 / r->GetHeight()) * 2) - 1.f;
			return;
		}
		switch(param1) {
		case VK_ADD: zoom(true); return;
		case VK_SUBTRACT: zoom(); return;
		case 0x57: camY -= 0.5f; return;
		case 0x53: camY += 0.5f; return;
		case 0x41: camX -= 0.5f; return;
		case 0x44: camX += 0.5f; return;
		case VK_CONTROL: game->SetScene(new Penis()); return;
		case VK_ESCAPE: PostQuitMessage(0); return;
		}
	}
};

int main() {
	Game(new TestScene(), "Testgame").start();
	return 1;
}
