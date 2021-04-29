#include "Game.h"
#include "utilities.h"

const char CLASS_NAME[] = "D2DGame";

Game* game;

void Timer(timerCallback func, bool& condition, float targetTime, float& procTime, int& perSec) {
	long64 t = timeNow();
	float timer = 0, secTimer = 0, psCtr = 0;

	while(condition) {

		float delta = deltaTime(t);
		t = timeNow();

		if(timer >= targetTime) {
			long64 d = timeNow();
			func(game, timer);
			procTime = deltaTime(d);
			psCtr++;
			timer = 0;
		}

		if(secTimer >= 1) {
			perSec = psCtr;
			psCtr = 0;
			secTimer = 0;
		}

		timer += delta;
		secTimer += delta;
	}
}

void RenderActive(Game* g, float) { g->RenderCurrentScene(); }

void UpdateActive(Game* g, float delta) { g->activeScene->update(delta); }

int Game::EngineThread() {
	activeScene->setup(this);
	CreateThread(0, 0, InvokeRender, this, 0, 0);
	Timer(UpdateActive, runEngine, 1.f / gameTPS, tickTime, currentTPS);
	return 0;
}

int Game::RenderThread() {
	float target = 0;
	if(targetFps > 0) target = 1.f / targetFps;
	Timer(RenderActive, runEngine, target, frameTime, currentFps);
	return 0;
}

LRESULT Game::WndMsg(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_DESTROY:
		runEngine = false;
		r->DiscardTarget();
		r->Destroy();
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:
		r = new Renderer(window);
		CreateThread(0, 0, InvokeEngine, this, 0, 0);
		return 0;

	case WM_KEYDOWN: activeScene->userinput(false, wParam, 0); return 0;
	case WM_LBUTTONDOWN: activeScene->userinput(false, wParam, lParam); return 0;

	case WM_MOUSEMOVE: activeScene->userinput(true, LOWORD(lParam), HIWORD(lParam)); return 0;

	case WM_SIZE: r->Resize(); return 0;
	}
	return DefWindowProcA(window, uMsg, wParam, lParam);
}

Game::Game(IScene* startingScene, const char* windowName) {
	game = this;
	WNDCLASS wc = {};

	wc.lpfnWndProc = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		Game* pThis = 0;
		if(uMsg == WM_NCCREATE) {
			pThis = (Game*)((CREATESTRUCT*)lParam)->lpCreateParams;
			pThis->window = hwnd;
			SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
		} else
			pThis = (Game*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

		if(pThis) return pThis->WndMsg(uMsg, wParam, lParam);
		else
			return DefWindowProcA(hwnd, uMsg, wParam, lParam);
	};

	wc.hInstance = GetModuleHandleA(0);
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursorA(0, IDC_ARROW);

	RegisterClassA(&wc);

	activeScene = startingScene;

	CreateWindowExA(0, CLASS_NAME, windowName, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0,
					0, GetModuleHandleA(0), this);

	if(window == 0) return;

	ShowWindow(window, 5);
}

void Game::SetScene(IScene* s) {
	activeScene = s;
	activeScene->setup(this);
}

void Game::RenderCurrentScene() {
	if(r->CreateTarget()) {
		r->BeginDraw();
		activeScene->render();
		if(!r->EndDraw()) r->DiscardTarget();
	}
}

int Game::start() {
	MSG msg;
	while(GetMessageA(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return 0;
}

void IScene::setup(Game* game) {
	this->game = game;
	r = game->r;
	init();
}
