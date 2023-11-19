#pragma once

#include "pch.h"
#include "Event.h"

class Window {
  public:
	virtual ~Window(){};
	virtual void* NativeHandle() = 0;
	virtual int width() = 0;
	virtual int height() = 0;
	function<void ()> OnResize = nullptr;
	function<void (const UserInputEvent&)> OnUserInput = nullptr;
	function<void ()> OnClose = nullptr;
};