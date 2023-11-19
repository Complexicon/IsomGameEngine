#pragma once

#include "Renderer.h"
#include "Event.h"

class Game;
class IScene {

  private:
	Game* _owner;

  protected:
	Game* game() { return _owner; }

  public:
	IScene(Game* owner) : _owner(owner) {}
	virtual void tick(float delta) = 0;
	virtual void render(Renderer* renderer) = 0;
	virtual void userinput(const UserInputEvent& event) = 0;
	virtual void resize() {};

};