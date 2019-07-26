#pragma once

#include "Common.h"

enum State {
	None,
	BlockFalling,
	Resolution,
	GameOver,
	Restart,
	Paused,
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

class GameBoard {
private:
	sf::RenderWindow* window;
	sf::Font gameFont;
	sf::Texture blocksSpriteSheet;
	sf::Texture backgroundImage;
	sf::Sound completionSound;

	std::chrono::milliseconds elapsedTimeForStep;

	Tile* board;
	int width;
	int height;
	int cellSize;
	int score;

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

	void swapBlocks(int x1, int y1, int x2, int y2);
	void setNoControl();

public:
	GameBoard(sf::RenderWindow* window, int boardWidth, int boardHeight);
	~GameBoard();

	std::chrono::milliseconds timestep;
	State state;

	void update(std::chrono::milliseconds elapsedTime);
	void render(std::chrono::milliseconds elapsedTime);

	void move(Direction direction);
	void rotate();
};