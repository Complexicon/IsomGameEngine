#pragma once

#include "pch.h"
#include "Window.h"
#include "Platform.h"
#include "IScene.h"
#include "Renderer.h"
#include "Font.h"

class Game {

	private:
		void Timer(function<void(float delta)> callback, bool& condition, uint32_t& targetRunsPerSecond, float& procTime, uint32_t& perSec);
		void RenderLoop();
		void GameLoop(float delta);

		thread rendering;
		thread gamelogic;

		float currentTickMS;
		uint32_t currentTPS;
		float currentRenderMS;
		uint32_t currentFPS;
		
		bool runEngine = true;

		Platform* platform = nullptr;
		Window* window = nullptr;

	public:
		Font defaultFont;
		Renderer* renderer = nullptr;
		bool debug = false;

		Window* GetWindow() { return window; }
		Platform* GetPlatform() { return platform; }

		IScene* activeScene = nullptr;
		uint32_t targetTPS = 64;
		uint32_t targetFPS = 60;
		bool vsync = false;
		string additionalDebugInfo;

		Game(string title, bool debug);
		void stop();
		void start();
};