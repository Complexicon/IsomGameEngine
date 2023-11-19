#ifdef _WIN32
#include "engine/Platform.h"
#include "pch.h"
#include "DirectX.h"
#include "Window.h"
#undef CreateWindow

class PlatformWin : public Platform {
  public:
	PlatformWin() {}
	Window* CreateWindow() {
		return new Win32Window();
	}

	Renderer* CreateRenderer(Window* window) {
		return DirectX::CreateMainTarget((HWND)window->NativeHandle());
	}

	void ProcessEvents() {
		MSG msg;
		while(GetMessageA(&msg, 0, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
	}

	uint64_t milis() {
		FILETIME t1;
		GetSystemTimeAsFileTime(&t1);
		return (uint64_t)t1.dwLowDateTime + ((uint64_t)(t1.dwHighDateTime) << 32LL);
	}

	void Quit() {
		PostQuitMessage(0);
	}

	void sleep(uint64_t milis) {
		Sleep(milis);
	}
};

Platform* Create() { return new PlatformWin(); }

#endif