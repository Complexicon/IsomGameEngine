#ifndef ISCENEHDR
#define ISCENEHDR

#include "UserInput.h"

class Game;
class Renderer;

class IScene {
  public:
	Game* game = 0;
	Renderer* r = 0;

	virtual void init(){};
	virtual void update(float delta) = 0;
	virtual void render() = 0;
	virtual void userinput(const UserInputEvent& event) = 0;

	friend class Game;
};

#endif