#pragma once
#define WIN32_LEAN_AND_MEAN
#undef UNICODE
#include "Renderer.h"
#include <windows.h>

#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

struct iV2D{ int x,y; };
struct fV2D{ float x,y; };

class Game;
class IScene;

typedef long long long64;
typedef void (*timerCallback)(Game*, float);

inline int random(int bounds) { return rand() % bounds; }
inline long64 timeNow() {
	FILETIME t1;
	GetSystemTimeAsFileTime(&t1);
	return (long64)t1.dwLowDateTime + ((long64)(t1.dwHighDateTime) << 32LL);
}
inline float deltaTime(long64 startTiming) { return (float)(timeNow() - startTiming) / 10000000; }

class Game {
  private:
	LRESULT WndMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static DWORD InvokeEngine(LPVOID gameInst) { return ((Game*)gameInst)->EngineThread(); };
	static DWORD InvokeRender(LPVOID gameInst) { return ((Game*)gameInst)->RenderThread(); };
	int EngineThread();
	int RenderThread();
	void Timer(timerCallback func, bool& condition, float targetTime, float& procTime, int& perSec);
	float tickTime = 0;
	float frameTime = 0;
	int currentFps = 0;
	int currentTPS = 0;

  public:
	Renderer* r;
	HWND window;
	IScene* activeScene;
	int targetFps = 0;
	int gameTPS = 20;
	bool runEngine = true;

	Game(IScene* startingScene, const char* windowName);
	void SetScene(IScene* s);
	void RenderCurrentScene();
	void start();

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