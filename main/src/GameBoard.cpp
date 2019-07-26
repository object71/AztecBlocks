#include "GameBoard.h"
#include <random>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(main);
int notThatValueByChance(int valueToAvoid);

using namespace std::chrono;

static std::random_device rng;
static std::uniform_int_distribution<int> range(1, 9);

static inline int position(int x, int y, int width) {
	return y * width + x;
}

GameBoard::GameBoard(sf::RenderWindow* window, int boardWidth, int boardHeight) {
	this->window = window;
	elapsedTimeForStep = milliseconds(0);
	state = State::None;
	width = boardWidth;
	height = boardHeight;
	timestep = milliseconds(700);
	cellSize = 48;
	score = 0;

	sf::SoundBuffer* buffer = new sf::SoundBuffer();
	cmrc::file completionSoundFile = cmrc::main::get_filesystem().open("resources/completionSound.wav");
	buffer->loadFromMemory(completionSoundFile.begin(), completionSoundFile.size());
	completionSound.setBuffer(*buffer);

	cmrc::file backgroundImageFile = cmrc::main::get_filesystem().open("resources/background.png");
	backgroundImage.loadFromMemory(backgroundImageFile.begin(), backgroundImageFile.size());

	cmrc::file blocksSpritesheetFile = cmrc::main::get_filesystem().open("resources/textureStone.png");
	blocksSpriteSheet.loadFromMemory(blocksSpritesheetFile.begin(), blocksSpritesheetFile.size());

	cmrc::file mainFontFile = cmrc::main::get_filesystem().open("resources/Boxy-Bold.ttf");
	gameFont.loadFromMemory(mainFontFile.begin(), mainFontFile.size());

	board = new Tile[width * height];
}

GameBoard::~GameBoard() {
	this->window = nullptr;

	delete completionSound.getBuffer();
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
				completionSound.play();
				state = State::BlockFalling;
			}
			break;
		case State::Restart:
			score = 0;
			board = new Tile[width * height];
			state = State::None;
			break;
		case State::GameOver:
			break;
		}
	}

}

void GameBoard::spawnBlocksOnTop() {
	int position = this->width / 2;
	if (this->board[position].value || this->board[position - 1].value) {
		state = State::GameOver;
	}
	else {
		int value1 = range(rng);
		int value2 = notThatValueByChance(value1);

		Tile tile1(value1);
		Tile tile2(value2);
		tile1.hasControl = true;
		tile2.hasControl = true;

		this->board[position] = tile1;
		this->board[position - 1] = tile2;

		state = State::BlockFalling;
	}
}

static int notThatValueByChance(int valueToAvoid) {
	if (range(rng) == 7) {
		return valueToAvoid;
	}
	else {
		int value;
		do {
			value = range(rng);
		} while (value == valueToAvoid);
		return value;
	}
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
		if (x < width - 1 && getValueAt(x + 1, y) == value) {
			setValueAt(x, y, 0);
			if (!propagateSetZeroValue(x + 1, y)) {
				setValueAt(x + 1, y, 0);
				score += value;
			}
			flag = true;
		}

		if (x > 0 && getValueAt(x - 1, y) == value) {
			setValueAt(x, y, 0);
			if (!propagateSetZeroValue(x - 1, y)) {
				setValueAt(x - 1, y, 0);
				score += value;
			}
			flag = true;
		}

		if (y < height - 1 && getValueAt(x, y + 1) == value) {
			setValueAt(x, y, 0);
			if (!propagateSetZeroValue(x, y + 1)) {
				setValueAt(x, y + 1, 0);
				score += value;
			}
			flag = true;
		}

		if (y > 0 && getValueAt(x, y - 1) == value) {
			setValueAt(x, y, 0);
			if (!propagateSetZeroValue(x, y - 1)) {
				setValueAt(x, y - 1, 0);
				score += value;
			}
			flag = true;
		}

	}

	if (flag) {
		score += value;
	}

	return flag;
}

int GameBoard::getValueAt(int x, int y) {
	return this->board[position(x, y, width)].value;
}

void GameBoard::setValueAt(int x, int y, int value) {
	if (!value) {
		this->board[position(x, y, width)].forDestruction = true;
	}
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

				if (x < width - 1 && getHasControlAt(x + 1, y)) {
					if (!getValueAt(x, y + 1)) {
						swapBlocks(x, y, x, y + 1);
						swapBlocks(x + 1, y, x, y);
						rotated = true;
					}
					else if (!getValueAt(x, y - 1)) {
						swapBlocks(x, y, x, y - 1);
						swapBlocks(x + 1, y, x, y);
						rotated = true;
					}
				}
				else if (getHasControlAt(x, y - 1)) {
					if (x < width - 1 && !getValueAt(x + 1, y)) {
						swapBlocks(x, y, x + 1, y);
						swapBlocks(x, y - 1, x, y);
						rotated = true;
					}
					else if (x > 0 && !getValueAt(x - 1, y)) {
						swapBlocks(x, y, x - 1, y);
						swapBlocks(x, y - 1, x, y);
						rotated = true;
					}
				}
			}

		}
	}
}

void GameBoard::render(milliseconds elapsedTime) {
	sf::RectangleShape background(sf::Vector2f(window->getSize()));
	background.setTexture(&backgroundImage);
	window->draw(background);

	int boardPixelWidth = cellSize * width;
	int boardPixelHeight = cellSize * height;

	int paddingLeft = (window->getSize().x - boardPixelWidth) / 2;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			Tile tile = getTileAt(x, y);
			int value = tile.value;

			sf::RectangleShape tileRectangle(sf::Vector2f(cellSize, cellSize));
			tileRectangle.setPosition(sf::Vector2f((x * cellSize) + paddingLeft, (y * cellSize) + 5));
			tileRectangle.setOutlineThickness(1);

			if (value) {
				sf::Text textValue;
				textValue.setFont(gameFont);
				textValue.setFillColor(sf::Color(255, 255, 255, 215));
				textValue.setString(std::to_string(value));

				textValue.setPosition(tileRectangle.getPosition().x + 10, tileRectangle.getPosition().y + 5);

				tileRectangle.setTexture(&blocksSpriteSheet);
				tileRectangle.setTextureRect(sf::IntRect(0, 0, 256, 256));
				tileRectangle.setOutlineColor(sf::Color::Black);


				window->draw(tileRectangle);
				window->draw(textValue);
			}
			else {
				tileRectangle.setFillColor(sf::Color(20, 20, 20, 100));
				tileRectangle.setOutlineColor(sf::Color::White);

				window->draw(tileRectangle);
			}
		}
	}

	sf::Text scoreText;
	scoreText.setFont(gameFont);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setString(std::to_string(score));
	scoreText.setCharacterSize(24);
	window->draw(scoreText);

	if (state == State::GameOver) {
		sf::RectangleShape overlay(sf::Vector2f(window->getSize()));
		overlay.setFillColor(sf::Color(0, 0, 0, 200));
		window->draw(overlay);

		sf::Text gameOverText;
		gameOverText.setFont(gameFont);
		gameOverText.setFillColor(sf::Color::White);
		gameOverText.setString("Game Over");
		gameOverText.setCharacterSize(48);
		gameOverText.setPosition((boardPixelWidth / 2), (boardPixelHeight / 2) - gameOverText.getCharacterSize());

		sf::Text restartText;
		restartText.setFont(gameFont);
		restartText.setFillColor(sf::Color::White);
		restartText.setString("press space to restart");
		restartText.setCharacterSize(24);
		restartText.setPosition((boardPixelWidth / 2), (boardPixelHeight / 2) + 6);

		window->draw(gameOverText);
		window->draw(restartText);
	}

	if (state == State::Paused) {
		sf::RectangleShape overlay(sf::Vector2f(window->getSize()));
		overlay.setFillColor(sf::Color(0, 0, 0, 200));
		window->draw(overlay);

		sf::Text pausedText;
		pausedText.setFont(gameFont);
		pausedText.setFillColor(sf::Color::White);
		pausedText.setString("Paused");
		pausedText.setCharacterSize(48);
		pausedText.setPosition((boardPixelWidth / 2), (boardPixelHeight / 2) - pausedText.getCharacterSize());

		window->draw(pausedText);
	}

}