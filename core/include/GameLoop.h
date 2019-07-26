#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

class GameLoop {
private:
	void (*update)(std::chrono::milliseconds);
	void (*render)();
	bool running;
	std::chrono::milliseconds msPerUpdate;
	int targetFPS;

	void init();
public:
	GameLoop();
	GameLoop(void(*update)(std::chrono::milliseconds), void(*render)());
	~GameLoop();

	bool getRunning();
	void setRunning(bool value);

	void runGameLoop();
};