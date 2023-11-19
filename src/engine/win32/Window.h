#pragma once

#include "engine/Platform.h"
#include "pch.h"

#undef UNICODE
#include "windows.h"

#define CLASS_NAME "GameEngineWindow"

class Win32Window : public Window {

	HWND windowHandle;

	function<void ()> OnResize = nullptr;
	friend class OpenGL;

	int w, h;

	LRESULT WndMsg(UINT uMsg, WPARAM wParam, LPARAM lParam) {

		switch(uMsg) {
		case WM_SIZE:
			RECT rc;
			GetClientRect(windowHandle, &rc);
			w = rc.right;
			h = rc.bottom;
			if(OnResize) OnResize();
			return 0;

		case WM_DESTROY:
		case WM_QUIT:
			if(OnClose) OnClose();
			return 0;

		case WM_KEYDOWN:
			if(OnUserInput) OnUserInput(KeyDownEvent((Key)wParam));
			return 0;
		case WM_KEYUP:
			if(OnUserInput) OnUserInput(KeyUpEvent((Key)wParam));
			return 0;

		case WM_LBUTTONDOWN:
			if(OnUserInput) OnUserInput(MouseClickEvent(MouseButton::Left, LOWORD(lParam), HIWORD(lParam), true));
			return 0;
		case WM_LBUTTONUP:
			if(OnUserInput) OnUserInput(MouseClickEvent(MouseButton::Left, LOWORD(lParam), HIWORD(lParam), false));
			return 0;

		case WM_MBUTTONDOWN:
			if(OnUserInput) OnUserInput(MouseClickEvent(MouseButton::Middle, LOWORD(lParam), HIWORD(lParam), true));
			return 0;
		case WM_MBUTTONUP:
			if(OnUserInput) OnUserInput(MouseClickEvent(MouseButton::Middle, LOWORD(lParam), HIWORD(lParam), false));
			return 0;

		case WM_RBUTTONDOWN:
			if(OnUserInput) OnUserInput(MouseClickEvent(MouseButton::Right, LOWORD(lParam), HIWORD(lParam), true));
			return 0;
		case WM_RBUTTONUP:
			if(OnUserInput) OnUserInput(MouseClickEvent(MouseButton::Right, LOWORD(lParam), HIWORD(lParam), false));
			return 0;

		case WM_MOUSEMOVE:
			if(OnUserInput) OnUserInput(MouseMoveEvent(LOWORD(lParam), HIWORD(lParam)));
			return 0;
			
		default: return DefWindowProcA(windowHandle, uMsg, wParam, lParam);
		}
	}

  public:
	Win32Window() {
		WNDCLASSA wc = {};

		const char* windowName = "";

		wc.lpfnWndProc = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			if(uMsg == WM_NCCREATE) {
				Win32Window* pThis = (Win32Window*)((CREATESTRUCT*)lParam)->lpCreateParams;
				pThis->windowHandle = hwnd;
				SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
				return DefWindowProcA(hwnd, uMsg, wParam, lParam);
			} else if(GetWindowLongPtrA(hwnd, GWLP_USERDATA))
				return ((Win32Window*)GetWindowLongPtrA(hwnd, GWLP_USERDATA))->WndMsg(uMsg, wParam, lParam);
			else
				return DefWindowProcA(hwnd, uMsg, wParam, lParam);
		};

		wc.hInstance = GetModuleHandleA(0);
		wc.lpszClassName = CLASS_NAME;
		wc.hCursor = LoadCursorA(0, IDC_ARROW);

		RegisterClassA(&wc);

		// GetSystemMetrics(SM_CXSCREEN)
		// GetSystemMetrics(SM_CYCREEN)
		//  WS_POPUP
		CreateWindowExA(0, CLASS_NAME, windowName, WS_OVERLAPPEDWINDOW, 0 + 200, 0 + 200, 1280, 720, 0, 0, GetModuleHandleA(0),
						this);

		if(windowHandle == 0) return;

		ShowWindow(windowHandle, 5);
	}

	void* NativeHandle() { return (void*)windowHandle; }

	int width() { return w; }

	int height() { return h; }
};