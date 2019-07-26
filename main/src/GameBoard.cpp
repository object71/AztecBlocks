#include "GameBoard.h"
#include <random>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(main);

using namespace std::chrono;

static std::default_random_engine rng;
static std::uniform_int_distribution<int> range(1, 6);

static inline int position(int x, int y, int width) {
	return y * width + x;
}

GameBoard::GameBoard(sf::RenderWindow* window, int boardWidth, int boardHeight) {
	this->window = window;
	elapsedTimeForStep = milliseconds(0);
	state = State::None;
	width = boardWidth;
	height = boardHeight;
	timestep = milliseconds(500);
	cellSize = 48;

	cmrc::file blocksSpritesheetFile = cmrc::main::get_filesystem().open("resources/dice_cubes_sd_bit.png");
	blocksSpriteSheet.loadFromMemory(blocksSpritesheetFile.begin(), blocksSpritesheetFile.size());

	cmrc::file mainFontFile = cmrc::main::get_filesystem().open("resources/UnicaOne-Regular.ttf");
	gameFont.loadFromMemory(mainFontFile.begin(), mainFontFile.size());

	board = new Tile[width * height];
}

GameBoard::~GameBoard() {
	this->window = nullptr;
	delete[] this->board;
}


void GameBoard::update(milliseconds elapsedTime) {
	elapsedTimeForStep += elapsedTime;

	if (elapsedTimeForStep >= timestep) {
		std::cout << elapsedTimeForStep.count() << std::endl;
		elapsedTimeForStep = milliseconds(0);

		switch (state) {
		case State::None:
			spawnBlocksOnTop();
			state = State::BlockFalling;
			break;
		case State::BlockFalling:
			if (!moveBlocks()) {
				state = State::Resolution;
			}
			break;
		case State::Resolution:
			if (!resolveBlocks()) {
				state = State::None;
			}
			else {
				state = State::BlockFalling;
			}
			break;
		}
	}

}

void GameBoard::spawnBlocksOnTop() {
	int position = this->width / 2;

	Tile tile1(range(rng));
	Tile tile2(range(rng));
	tile1.hasControl = true;
	tile2.hasControl = true;

	this->board[position] = tile1;
	this->board[position - 1] = tile2;
}

bool GameBoard::resolveBlocks() {
	bool flag = false;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			flag = propagateSetZeroValue(x, y) || flag;
		}
	}

	return flag;
}

bool GameBoard::propagateSetZeroValue(int x, int y) {
	int value = getValueAt(x, y);
	bool flag = false;

	if (value) {
		if (getValueAt(x + 1, y) == value) {
			setValueAt(x, y, 0);
			propagateSetZeroValue(x + 1, y);
			setValueAt(x + 1, y, 0);
			flag = true;
		}

		if (getValueAt(x - 1, y) == value) {
			setValueAt(x, y, 0);
			propagateSetZeroValue(x - 1, y);
			setValueAt(x - 1, y, 0);
			flag = true;
		}

		if (getValueAt(x, y + 1) == value) {
			setValueAt(x, y, 0);
			propagateSetZeroValue(x, y + 1);
			setValueAt(x, y + 1, 0);
			flag = true;
		}

		if (getValueAt(x, y - 1) == value) {
			setValueAt(x, y, 0);
			propagateSetZeroValue(x, y - 1);
			setValueAt(x, y - 1, 0);
			flag = true;
		}

	}

	return flag;
}

int GameBoard::getValueAt(int x, int y) {
	return this->board[position(x, y, width)].value;
}

void GameBoard::setValueAt(int x, int y, int value) {
	this->board[position(x, y, width)].value = value;
}

bool GameBoard::getHasControlAt(int x, int y) {
	return this->board[position(x, y, width)].hasControl;
}

void GameBoard::setHasControlAt(int x, int y, bool hasControl) {
	this->board[position(x, y, width)].hasControl = hasControl;
}

Tile GameBoard::getTileAt(int x, int y) {
	return this->board[position(x, y, width)];
}

void GameBoard::setTileAt(int x, int y, Tile tile) {
	this->board[position(x, y, width)] = tile;
}

void GameBoard::swapBlocks(int x1, int y1, int x2, int y2) {
	Tile tile1 = getTileAt(x1, y1);
	Tile tile2 = getTileAt(x2, y2);

	setTileAt(x1, y1, tile2);
	setTileAt(x2, y2, tile1);
}


bool GameBoard::moveBlocks() {
	bool somethingMoved = false;

	for (int x = 0; x < width; x++) {
		for (int y = height - 2; y >= 0; y--) {
			int value = this->getValueAt(x, y);
			int valueBelow = this->getValueAt(x, y + 1);

			if (value && !valueBelow) {
				this->swapBlocks(x, y, x, y + 1);
				somethingMoved = true;

				if (y + 1 == height - 1) {
					setNoControl();
				}
			}
			else if (getHasControlAt(x, y)) {
				setNoControl();
			}
		}
	}

	return somethingMoved;
}

void GameBoard::setNoControl() {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			setHasControlAt(x, y, false);
		}
	}
}

void GameBoard::move(Direction direction) {
	if (direction == Direction::Left) {
		for (int x = 1; x < width; x++) {
			for (int y = 0; y < height; y++) {

				if (getHasControlAt(x, y) && !getValueAt(x - 1, y)) {
					swapBlocks(x, y, x - 1, y);
				}

			}
		}
	}
	else if (direction == Direction::Right) {
		for (int x = width - 2; x >= 0; x--) {
			for (int y = 0; y < height; y++) {

				if (getHasControlAt(x, y) && !getValueAt(x + 1, y)) {
					swapBlocks(x, y, x + 1, y);
				}

			}
		}
	}

}

void GameBoard::rotate() {
	bool rotated = false;
	for (int x = 0; x < width && !rotated; x++) {
		for (int y = 1; y < height && !rotated; y++) {

			if (getHasControlAt(x, y)) {

				if (x > 0 && getHasControlAt(x - 1, y)) {
					if (!getValueAt(x, y + 1)) {
						swapBlocks(x, y, x, y + 1);
						swapBlocks(x - 1, y, x, y);
						rotated = true;
					}
					else if (!getValueAt(x, y - 1)) {
						swapBlocks(x, y, x, y - 1);
						swapBlocks(x - 1, y, x, y);
						rotated = true;
					}
				}
				else if (getHasControlAt(x, y - 1)) {
					if (x > 0 && !getValueAt(x - 1, y)) {
						swapBlocks(x, y, x - 1, y);
						swapBlocks(x, y - 1, x, y);
						rotated = true;
					}
					else if (x < width - 1 && !getValueAt(x + 1, y)) {
						swapBlocks(x, y, x + 1, y);
						swapBlocks(x, y - 1, x, y);
						rotated = true;
					}
				}
			}

		}
	}
}


void GameBoard::render() {

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int value = getValueAt(x, y);

			sf::RectangleShape shape(sf::Vector2f(cellSize, cellSize));
			shape.setTexture(&blocksSpriteSheet);
			shape.setTextureRect(sf::IntRect((value - 1) * 32, 0, 32, 32));

			shape.setOutlineThickness(1);

			if (value) {
				shape.setFillColor(sf::Color::White);
				shape.setOutlineColor(sf::Color::Black);
			}
			else {
				shape.setFillColor(sf::Color::Black);
				shape.setOutlineColor(sf::Color::White);
			}

			shape.setPosition(sf::Vector2f(x * cellSize, y * cellSize));

			window->draw(shape);
		}
	}
}