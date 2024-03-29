﻿#include "GameBoard.h"
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

GameBoard::GameBoard(sf::RenderWindow& t_window, const int& boardWidth, const int& boardHeight)
	: m_window(t_window)
{
	elapsedTimeForStep = milliseconds(0);
	state = BoardState::Initial;
	width = boardWidth;
	height = boardHeight;
	timestep = milliseconds(700);
	cellSize = 48;
	score = 0;
	mute = false;

	sf::SoundBuffer* buffer = new sf::SoundBuffer();
	cmrc::file completionSoundFile = cmrc::main::get_filesystem().open("resources/completionSound.wav");
	buffer->loadFromMemory(completionSoundFile.begin(), completionSoundFile.size());
	completionSound.setBuffer(*buffer);

	sf::SoundBuffer* musicBuffer = new sf::SoundBuffer();
	cmrc::file gameMusicFile = cmrc::main::get_filesystem().open("resources/gameMusic.ogg");
	musicBuffer->loadFromMemory(gameMusicFile.begin(), gameMusicFile.size());
	gameMusic.setBuffer(*musicBuffer);
	gameMusic.setLoop(true);
	gameMusic.setVolume(25);
	gameMusic.play();

	cmrc::file backgroundImageFile = cmrc::main::get_filesystem().open("resources/background.png");
	backgroundImage.loadFromMemory(backgroundImageFile.begin(), backgroundImageFile.size());

	cmrc::file blocksSpritesheetFile = cmrc::main::get_filesystem().open("resources/textureStone.png");
	blocksSpriteSheet.loadFromMemory(blocksSpritesheetFile.begin(), blocksSpritesheetFile.size());

	cmrc::file mainFontFile = cmrc::main::get_filesystem().open("resources/Boxy-Bold.ttf");
	gameFont.loadFromMemory(mainFontFile.begin(), mainFontFile.size());

	board = new Tile[width * height];
}

GameBoard::~GameBoard() {
	delete this->gameMusic.getBuffer();
	delete this->completionSound.getBuffer();
	delete[] this->board;
}


void GameBoard::update(milliseconds elapsedTime) {
	if (m_window.isOpen()) {
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				// TODO: Find how to close properly
				//setRunning(false);
			}
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Down) {
					update(timestep);
				}
			}
			else if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::Left) {
					move(Direction::Left);
				}
				else if (event.key.code == sf::Keyboard::Right) {
					move(Direction::Right);
				}
				else if (event.key.code == sf::Keyboard::Space) {
					if (state == BoardState::GameOver || state == Initial) {
						state = BoardState::Restart;
					}
					else {
						rotate();
					}
				}
				else if (event.key.code == sf::Keyboard::M) {
					setMute(getMute() ? false : true);
				}
				else if (event.key.code == sf::Keyboard::Escape) {
					if (state != BoardState::Paused) {
						state = BoardState::Paused;
					}
					else {
						state = BoardState::BlockFalling;
					}
				}
			}
		}
	}

	elapsedTimeForStep += elapsedTime;

	if (elapsedTimeForStep >= timestep) {
		elapsedTimeForStep = milliseconds(0);

		switch (state) {
		case BoardState::None:
			spawnBlocksOnTop();
			break;
		case BoardState::BlockFalling:
			if (!moveBlocks()) {
				state = BoardState::Resolution;
			}
			break;
		case BoardState::Resolution:
			if (!resolveBlocks()) {
				state = BoardState::None;
			}
			else {
				if (!mute) {
					completionSound.play();
				}
				state = BoardState::BlockFalling;
			}
			break;
		case BoardState::Restart:
			score = 0;
			board = new Tile[width * height];
			state = BoardState::None;
			break;
		case BoardState::GameOver:
		case BoardState::Initial:
		case BoardState::Paused:
			break;
		}
	}

}

void GameBoard::spawnBlocksOnTop() {
	int position = this->width / 2;
	if (this->board[position].value || this->board[position - 1].value) {
		state = BoardState::GameOver;
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

		state = BoardState::BlockFalling;
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

void GameBoard::draw(milliseconds elapsedTime) {
	sf::Vector2u windowSize = m_window.getSize();
	sf::RectangleShape background = sf::RectangleShape(sf::Vector2f(windowSize));
	background.setTexture(&backgroundImage);
	m_window.draw(background);

	int boardPixelWidth = cellSize * width;
	int boardPixelHeight = cellSize * height;

	int paddingLeft = (m_window.getSize().x - boardPixelWidth) / 2;
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			Tile tile = getTileAt(x, y);
			int value = tile.value;

			sf::RectangleShape tileRectangle(sf::Vector2f(cellSize, cellSize));
			tileRectangle.setPosition(sf::Vector2f((x * cellSize) + paddingLeft, (y * cellSize) + 5));
			tileRectangle.setOutlineThickness(1);

			if (value) {
				sf::Text textValue = generateTextWithGameFont(std::to_string(value), 
					tileRectangle.getPosition().x + 10, 
					tileRectangle.getPosition().y + 5, 
					sf::Color(255, 255, 255, 215));

				tileRectangle.setTexture(&blocksSpriteSheet);
				tileRectangle.setTextureRect(sf::IntRect(0, 0, 256, 256));
				tileRectangle.setOutlineColor(sf::Color::Black);


				m_window.draw(tileRectangle);
				m_window.draw(textValue);
			}
			else {
				tileRectangle.setFillColor(sf::Color(20, 20, 20, 100));
				tileRectangle.setOutlineColor(sf::Color::White);

				m_window.draw(tileRectangle);
			}
		}
	}

	sf::Text scoreText;
	scoreText.setFont(gameFont);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setString(std::to_string(score));
	scoreText.setCharacterSize(24);
	m_window.draw(scoreText);

	if (state == BoardState::GameOver) {
		sf::RectangleShape overlay(sf::Vector2f(m_window.getSize()));
		overlay.setFillColor(sf::Color(0, 0, 0, 200));
		m_window.draw(overlay);

		sf::Text gameOverText = generateTextWithGameFont("Game Over", 0, 0, sf::Color::White, 48);
		gameOverText.setPosition((windowSize.x / 2) - (gameOverText.getGlobalBounds().width / 2), (windowSize.y / 2) - gameOverText.getCharacterSize());

		sf::Text restartText = generateTextWithGameFont("Press SPACE to restart", 0, 0, sf::Color::White, 24);
		restartText.setPosition((windowSize.x / 2) - (restartText.getGlobalBounds().width / 2), (windowSize.y / 2) + 6);

		m_window.draw(gameOverText);
		m_window.draw(restartText);
	}

	if (state == BoardState::Paused) {
		sf::RectangleShape overlay(sf::Vector2f(m_window.getSize()));
		overlay.setFillColor(sf::Color(0, 0, 0, 200));
		m_window.draw(overlay);

		sf::Text pausedText = generateTextWithGameFont("Paused", 0, 0, sf::Color::White, 48);
		pausedText.setPosition((windowSize.x / 2) - (pausedText.getGlobalBounds().width / 2), (windowSize.y / 2) - pausedText.getCharacterSize());
		m_window.draw(pausedText);
	}

	if (state == BoardState::Initial) {
		sf::RectangleShape overlay(sf::Vector2f(m_window.getSize()));
		overlay.setFillColor(sf::Color(0, 0, 0, 200));
		m_window.draw(overlay);

		sf::Text pausedText = generateTextWithGameFont("LEFT/RIGHT to move the falling block\nSPACE to rotate\nDOWN to speed up\nESC for pause\nM for mute", 0, 0, sf::Color::White, 24);
		pausedText.setPosition((windowSize.x / 2) - (pausedText.getGlobalBounds().width / 2), (windowSize.y / 2) - (pausedText.getGlobalBounds().height / 2));
		pausedText.setLineSpacing(1.5);
		m_window.draw(pausedText);
	}
}

sf::Text GameBoard::generateTextWithGameFont(std::string text, int x, int y, sf::Color color, int characterSize) {
	sf::Text textElement;
	textElement.setFont(gameFont);
	textElement.setFillColor(color);
	textElement.setString(text);
	textElement.setCharacterSize(characterSize);
	textElement.setPosition(x, y);

	return textElement;
}

bool GameBoard::getMute() {
	return mute;
}

void GameBoard::setMute(bool value) {
	if (value) {
		gameMusic.pause();
	}
	else {
		gameMusic.play();
	}

	mute = value;
}