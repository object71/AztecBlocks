#include "GameLoop.h"
#include <thread>

using namespace std::chrono;

static milliseconds getCurrentTime() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

void GameLoop::init() {
	running = false;
	msPerUpdate = milliseconds(100);
	targetFPS = 60;
	update = nullptr;
	render = nullptr;
}

GameLoop::GameLoop() {
	init();
}

GameLoop::GameLoop(void(*update)(milliseconds), void(*render)()) {
	init();
	this->update = update;
	this->render = render;
}

GameLoop::~GameLoop() {
}

void GameLoop::runGameLoop() {
	running = true;

	milliseconds previous = getCurrentTime();
	milliseconds lag(0);
	while (running)
	{
		milliseconds current = getCurrentTime();
		milliseconds elapsed = current - previous;
		previous = current;
		lag += elapsed;

		while (lag >= msPerUpdate)
		{
			this->update(msPerUpdate);
			lag -= msPerUpdate;
		}

		this->render();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / targetFPS));
	}
}

bool GameLoop::getRunning() { return running; }
void GameLoop::setRunning(bool value) { running = value; }