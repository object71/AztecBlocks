#pragma once

#include <chrono>
#include <memory>
#include "Interfaces.h"

class GameLoop {
private:
	bool m_running;
	std::chrono::milliseconds m_step;
	IFullComponent& m_mainComponent;
public:
	GameLoop(IFullComponent& component);
	~GameLoop();

	bool getRunning();
	void setRunning(const bool& value);

	std::chrono::milliseconds getStep();
	void setStep(const std::chrono::milliseconds& value);

	void run();
};