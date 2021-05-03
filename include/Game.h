#pragma once
#define WIN32_LEAN_AND_MEAN
#undef UNICODE
#include "Renderer.h"
#include <windows.h>

#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

// integer Vector2D
struct iV2D{ int x,y; };
// float Vector2D
struct fV2D{ float x,y; };

class Game;
class IScene;

typedef void (*timerCallback)(Game*, float);

class Game {
  private:
	LRESULT WndMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static unsigned long InvokeEngine(void* gameInst) { return ((Game*)gameInst)->EngineThread(); };
	static unsigned long InvokeRender(void* gameInst) { return ((Game*)gameInst)->RenderThread(); };
	int EngineThread();
	int RenderThread();
	float tickTime = 0;
	float frameTime = 0;
	int currentFps = 0;
	int currentTPS = 0;

  public:
	Renderer* r;
	HWND window;
	IScene* activeScene;
	int targetFps = 0;
	int gameTPS = 0;
	bool runEngine = true;

	Game(IScene* startingScene, const char* windowName);
	IScene* InitScene(IScene* s);
	void RenderCurrentScene();
	int start();

	int getFPS() { return currentFps; }
	int getTPS() { return currentTPS; }
	float getTickTime() { return tickTime; }
	float getFrameTime() { return frameTime; }
};

class IScene {
  private:
	void setup(Game* game);

  public:
	Game* game = 0;
	Renderer* r = 0;

	virtual void init(){};
	virtual void update(float delta) = 0;
	virtual void render() = 0;
	virtual void userinput(bool mouseEvent, int param1, int param2) = 0;

	friend class Game;
};