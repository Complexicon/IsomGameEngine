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

		perSec = perSec * (!(int)secTimer) + psCtr * (int)secTimer; // branchless hack
		psCtr -= psCtr * (int)secTimer;								// branchless hack
		secTimer -= secTimer * (int)secTimer;						// branchless hack

		timer += delta;
		secTimer += delta;
	}
}

void RenderActive(Game* g, float) { g->RenderCurrentScene(); }

void UpdateActive(Game* g, float delta) { g->activeScene->update(delta); }

int Game::EngineThread() {
	InitScene(activeScene);
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

void Game::StopEngine() {
	runEngine = false;
	r->DiscardTarget();
	r->Destroy();
	PostQuitMessage(0);
}

LRESULT Game::WndMsg(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_DESTROY:
	case WM_QUIT: StopEngine();return 0;

	case WM_CREATE:
		r = new Renderer(window);
		CreateThread(0, 0, InvokeEngine, this, 0, 0);
		return 0;

	case WM_KEYDOWN: activeScene->userinput(KeyDownEvent((Key)wParam)); return 0;
	case WM_KEYUP: activeScene->userinput(KeyUpEvent((Key)wParam)); return 0;

	case WM_LBUTTONDOWN:
		activeScene->userinput(MouseClickEvent(MouseButton::Left, LOWORD(lParam), HIWORD(lParam), true));
		return 0;
	case WM_LBUTTONUP:
		activeScene->userinput(MouseClickEvent(MouseButton::Left, LOWORD(lParam), HIWORD(lParam), false));
		return 0;

	case WM_MBUTTONDOWN:
		activeScene->userinput(MouseClickEvent(MouseButton::Middle, LOWORD(lParam), HIWORD(lParam), true));
		return 0;
	case WM_MBUTTONUP:
		activeScene->userinput(MouseClickEvent(MouseButton::Middle, LOWORD(lParam), HIWORD(lParam), false));
		return 0;

	case WM_RBUTTONDOWN:
		activeScene->userinput(MouseClickEvent(MouseButton::Right, LOWORD(lParam), HIWORD(lParam), true));
		return 0;
	case WM_RBUTTONUP:
		activeScene->userinput(MouseClickEvent(MouseButton::Right, LOWORD(lParam), HIWORD(lParam), false));
		return 0;

	case WM_MOUSEMOVE: activeScene->userinput(MouseMoveEvent(LOWORD(lParam), HIWORD(lParam))); return 0;

	case WM_SIZE: r->Resize(); return 0;
	}
	return DefWindowProcA(window, uMsg, wParam, lParam);
}

Game::Game(IScene* startingScene, const char* windowName) {
	game = this;
	WNDCLASS wc = {};

	wc.lpfnWndProc = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if(uMsg == WM_NCCREATE) {
			Game* pThis = (Game*)((CREATESTRUCT*)lParam)->lpCreateParams;
			pThis->window = hwnd;
			SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
			return DefWindowProcA(hwnd, uMsg, wParam, lParam);
		} else if(GetWindowLongPtrA(hwnd, GWLP_USERDATA))
			return ((Game*)GetWindowLongPtrA(hwnd, GWLP_USERDATA))->WndMsg(uMsg, wParam, lParam);
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

IScene* Game::InitScene(IScene* s) {
	s->game = this;
	s->r = r;
	s->init();
	return s;
}

void Game::RenderCurrentScene() {
	if(r->CreateTarget()) {
		r->BeginDraw();
		activeScene->render();
		if(drawDebug) {
			Str debugStr = str("fps: " + currentFps + "\n");
			debugStr += str("tps: " + currentTPS + "\n");
			debugStr += str("frametime: " + frameTime * 1000 + "ms\n");
			debugStr += str("tickTime: " + tickTime * 1000 + "ms\n");
			debugStr += additionalDebugInfo;
			r->FillRect(0, 0, 250, 150, Color(0, 100));
			r->DrawString(0, 0, 250, 150, Color(0xffffff), debugStr.c_str());
		}
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
