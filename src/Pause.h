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

	void userinput(const UserInputEvent& e){
		switch(e.type){
			case UserInputType::Mouse:
				if(CAST(MouseInputEvent, e).mouseEvent == MouseEventType::Move){
					MouseMoveEvent me = CAST(MouseMoveEvent, e);
					overContinue = IN_BOX(me.mouseX, me.mouseY, 200, 100, 500, 500);
					overQuit = IN_BOX(me.mouseX, me.mouseY, 200, 100, 500, 650);
				}else{
					MouseClickEvent me = CAST(MouseClickEvent, e);
					if(!me.pressed && me.button != MouseButton::Left) return;
					if(overContinue) game->activeScene = oldScene;
					else if(overQuit) PostQuitMessage(0);
				}
				return;
			case UserInputType::Keyboard:
				KeyboardInputEvent ke = CAST(KeyboardInputEvent, e);
				if(!ke.pressed) return;
				switch(ke.key) {
				case Key::ESCAPE:
					game->activeScene = oldScene;
					return;
				}
				return;
		}
	}

	public:
		PauseMenu(IScene* old) : oldScene(old) {}
};

#endif