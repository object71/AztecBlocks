#pragma once

#include <chrono>
#include <memory>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Interfaces.h"

enum State {
	None,
	BlockFalling,
	Resolution,
	GameOver,
	Restart,
	Paused,
	Initial,
};

enum Direction {
	Left,
	Right,
};

struct Tile {
	int value;
	bool hasControl;
	bool forDestruction;

	Tile() {
		value = 0;
		hasControl = false;
		forDestruction = false;
	}

	Tile(int value) {
		this->value = value;
		hasControl = false;
		forDestruction = false;
	}
};

class GameBoard: public IFullComponent {
private:
	sf::RenderWindow& m_window;
	sf::Font gameFont;
	sf::Texture blocksSpriteSheet;
	sf::Texture backgroundImage;
	sf::Sound completionSound;
	sf::Sound gameMusic;

	std::chrono::milliseconds elapsedTimeForStep;

	Tile* board;
	int width;
	int height;
	int cellSize;
	int score;
	bool mute;

	void spawnBlocksOnTop();
	bool moveBlocks();
	bool resolveBlocks();
	bool propagateSetZeroValue(int x, int y);

	int getValueAt(int x, int y);
	void setValueAt(int x, int y, int value);
	bool getHasControlAt(int x, int y);
	void setHasControlAt(int x, int y, bool value);
	Tile getTileAt(int x, int y);
	void setTileAt(int x, int y, Tile tile);
	sf::Text generateTextWithGameFont(std::string text, int x = 0, int y = 0, sf::Color color = sf::Color::White, int characterSize = 30);
	void swapBlocks(int x1, int y1, int x2, int y2);
	void setNoControl();

public:
	GameBoard(sf::RenderWindow& t_window, const int& boardWidth, const int& boardHeight);
	~GameBoard();

	std::chrono::milliseconds timestep;
	State state;

	bool getMute();
	void setMute(bool value);

	virtual void update(std::chrono::milliseconds elapsedTime) override;
	virtual void draw(std::chrono::milliseconds elapsedTime) override;

	void move(Direction direction);
	void rotate();
};
