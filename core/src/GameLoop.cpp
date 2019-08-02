#include "GameLoop.h"
#include <thread>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace std::chrono;

static milliseconds getCurrentTime() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

GameLoop::GameLoop(IFullComponent& component):
	m_mainComponent(component)
{
	m_running = false;
	m_step = std::chrono::milliseconds(10);
}

GameLoop::~GameLoop() {
}

void GameLoop::run() {
	m_running = true;

	milliseconds previous = getCurrentTime();
	milliseconds lag(0);
	while (m_running)
	{
		milliseconds current = getCurrentTime();
		milliseconds elapsed = current - previous;
		previous = current;
		lag += elapsed;

		while (lag >= m_step)
		{
			m_mainComponent.update(m_step);
			lag -= m_step;
		}

		m_mainComponent.draw(elapsed);
	}
}

bool GameLoop::getRunning() { return m_running; }

void GameLoop::setRunning(const bool& value) { m_running = value; }

std::chrono::milliseconds GameLoop::getStep() { return m_step; }

void GameLoop::setStep(const std::chrono::milliseconds& value) { m_step = value; }