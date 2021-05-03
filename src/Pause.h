#ifndef PAUSEMENUH
#define PAUSEMENUH

#include "Game.h"

#define IN_BOX(x,y,szX,szY,bX,bY) (x - bX >= 0 && x - bX <= szX && y - bY >= 0 && y - bY <= szY)

class PauseMenu : public IScene {

	IScene* oldScene;
	bool overContinue = false;
	bool overQuit = false;

	void update(float d) {}

	void render() {
		oldScene->render();
		r->FillRect(0, 0, r->GetWidth(), r->GetHeight(), Color(0, 100));

		r->DrawRect(500, 500, 200, 100, Color(0xdddddd));
		if(overContinue) r->FillRect(500, 500, 200, 100, Color(0xadadad, 150));
		r->DrawString(500, 500, 200, 100, Color(0xffffff), "Continue");

		r->DrawRect(500, 650, 200, 100, Color(0xdddddd));
		if(overQuit) r->FillRect(500, 650, 200, 100, Color(0xadadad, 150));
		r->DrawString(500, 650, 200, 100, Color(0xffffff), "Quit");

	}

	void userinput(bool mouseEvent, int param1, int param2) {
		if(mouseEvent){
			overContinue = IN_BOX(param1, param2, 200, 100, 500, 500);
			overQuit = IN_BOX(param1, param2, 200, 100, 500, 650);
		} else {
			switch(param1){
				case VK_ESCAPE:
					game->activeScene = oldScene;
					return;
				case VK_LBUTTON:
					if(overContinue) game->activeScene = oldScene;
					else if(overQuit) PostQuitMessage(0);
					return;
			}
		}
	}

	public:
		PauseMenu(IScene* old) : oldScene(old) {}
};

#endif