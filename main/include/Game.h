#pragma once

#include <chrono>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "Interfaces.h"
#include "GameBoard.h"

class Game : public IFullComponent {
private:
	GameBoard m_board;
	sf::RenderWindow m_window;
public:
	Game();
	~Game();

	virtual void update(std::chrono::milliseconds elapsedTime) override;
	virtual void draw(std::chrono::milliseconds elapsedTime) override;
};
