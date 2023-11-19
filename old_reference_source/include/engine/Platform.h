#pragma once
#include "pch.h"
#include "Window.h"
#include "Renderer.h"

class Platform {
  public:
	virtual ~Platform() {};
	virtual Window* CreateWindow() = 0;
	virtual Renderer* CreateRenderer(Window*) = 0;
	virtual void ProcessEvents() = 0;
	virtual void Quit() = 0;
	
	virtual uint64_t milis() = 0;
	virtual void sleep(uint64_t milis) = 0;
};

Platform* Create();