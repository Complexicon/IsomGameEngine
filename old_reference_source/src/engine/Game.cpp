#include "engine/Game.h"

void Game::Timer(function<void(float delta)> callback, bool& condition, uint32_t& targetRunsPerSecond, float& procTime,
				 uint32_t& perSec) {

	auto deltaTime = [&](uint64_t start) { return (float)(platform->milis() - start) / 10000000; };

	uint64_t t = platform->milis();
	float timer = 0, secTimer = 0, psCtr = 0;

	while(condition) {

		float target = 1.f / targetRunsPerSecond;

		float delta = deltaTime(t);
		t = platform->milis();

		if(timer >= target) {
			uint64_t d = platform->milis();
			callback(timer);
			procTime = deltaTime(d);
			psCtr++;
			timer -= target;
		}

		perSec = perSec * (!(int)secTimer) + psCtr * (int)secTimer; // branchless hack
		psCtr -= psCtr * (int)secTimer;								// branchless hack
		secTimer -= secTimer * (int)secTimer;						// branchless hack

		timer += delta;
		secTimer += delta;
		int sleepCompensation = ((target - timer) / 2) * 1000;
		platform->sleep(sleepCompensation < 0 ? 0 : sleepCompensation);
	}
}

void Game::stop() {
	runEngine = false;
	rendering.join();
	gamelogic.join();
	delete renderer;
	platform->Quit();
}

void Game::RenderLoop() {
	renderer->BeginDraw();
	// pre
	if(activeScene) activeScene->render(renderer);
	// post
	if(debug) {
		string debugText;
		debugText += string("fps: ") + std::to_string(currentFPS) + "\n";
		debugText += string("tps: ") + std::to_string(currentTPS) + "\n";
		debugText += string("frametime: ") + std::to_string(currentRenderMS) + "\n";
		debugText += string("ticktime: ") + std::to_string(currentTickMS) + "\n";
		debugText += additionalDebugInfo;

		renderer->FillRect(0, 0, 250, 150, Color(0, 100));
		renderer->DrawString(0, 0, 250, 150, Color(0xffffff), debugText);
	}

	if(!renderer->EndDraw()) throw runtime_error("failed to end draw call");

	if(scheduledResize) {
		renderer->Resize();
		scheduledResize = false;
	}
}

void Game::GameLoop(float delta) {
	// pre
	if(activeScene) activeScene->tick(delta);
	// post
}

Game::Game(string title, bool debug) {
	this->debug = debug;
	platform = Create();
	window = platform->CreateWindow();

	renderer = platform->CreateRenderer(window);

	window->OnResize = [&]() {
		scheduledResize = true;
		if(activeScene) activeScene->resize();
	};
	
	window->OnClose = [&]() { stop(); };

	window->OnUserInput = [&](const UserInputEvent& event) {
		if(activeScene) activeScene->userinput(event);
	};

}

void Game::start() {

	gamelogic = thread([&]() {
		Timer([&](float delta) { GameLoop(delta); }, this->runEngine, this->targetTPS, this->currentTickMS, this->currentTPS);
	});

	rendering = thread([&]() {
		Timer([&](float) { RenderLoop(); }, this->runEngine, this->targetFPS, this->currentRenderMS, this->currentFPS);
	});

	platform->ProcessEvents();
}
