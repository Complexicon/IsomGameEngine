#include "Game.h"
#include "Map.h"
#include "Str.h"
#include <windows.h>

#define IN_BOUNDS(vec, r, szX, szY) (vec.x - r >= 0 && vec.x + r <= szX && vec.y - r >= 0 && vec.y + r <= szY)
int speed = 400;

class TestScene : public IScene {

	Map m;

	void init() {
		game->gameTPS = 128;
		m.Randomize();
	}

	void update(float delta) {
		int w = r->GetWidth();
		int h = r->GetHeight();
	}

	void debug() {
		Str defaultDebug =
			strn("fps: " + game->getFPS() + "\ntps: " + game->getTPS() + "\nframetime: " + game->getFrameTime() * 1000 +
				 "ms\ntickTime: " + game->getTickTime() * 1000 + "ms");
		r->FillRoundedRect(0, 0, 200, 150, Color(0x0, 100), 0);
		r->DrawString(0, 0, 200, 150, Color(0xffffff), defaultDebug.c_str());
	}

	void render() {
		r->Clear(Color::random());

		/*
		for (int i = 0; i < m.GetMapHeight(); i++) {

			for (int j = 0; j < m.GetMapWidth(); j++) {
				switch (m.GetMap()[(i * m.GetMapHeight()) + j]) {
				case 0:
					r->DrawPixel(i, j, Color(0x7a5a00));
					break;
				case 1:
					r->DrawPixel(i, j, Color(0x18b300));
					break;
				case 2:
					r->DrawPixel(i, j, Color(0x2b56ff));
					break;
				}
			}

		}
		*/

		// debug overlay
		debug();
	}

	void userinput(bool mouseEvent, int param1, int param2) {
		if(mouseEvent) return;
		switch(param1) {
		case VK_ESCAPE: PostQuitMessage(0); return;
		case VK_CONTROL: return;
		}
	}
};

int main() { Game(new TestScene(), "Testgame").start(); }
