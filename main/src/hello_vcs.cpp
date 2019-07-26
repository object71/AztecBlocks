#include <cmrc/cmrc.hpp>
#include "Common.h"
#include <thread>
#include "GameLoop.h"
#include "GameBoard.h"
#if WINDOWS && !DEBUG
#include <windows.h>
#endif

using namespace std::chrono;

void update(milliseconds elapsedTime);
void render();
void run();

GameLoop* game;
GameBoard* board;
sf::RenderWindow* window;

int main(int argc, char* argv[]) {
#if WINDOWS && !DEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	std::thread gameLoop(&run);
	gameLoop.join();

	return 0;
}

static void run() {
	window = new sf::RenderWindow(sf::VideoMode(640, 480), "Aztec Blocks", sf::Style::Titlebar | sf::Style::Close);

	board = new GameBoard(window, 5, 9);
	game = new GameLoop(&update, &render);
	game->runGameLoop();

	window->close();

	delete window;
	delete game;
	delete board;
}

static void update(milliseconds elapsedTime) {

	if (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed ||
				(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				game->setRunning(false);
			}
			else if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::Left) {
					board->move(Direction::Left);
				}
				else if (event.key.code == sf::Keyboard::Right) {
					board->move(Direction::Right);
				}
				else if (event.key.code == sf::Keyboard::Space) {
					board->rotate();
				}
				else if (event.key.code == sf::Keyboard::Down) {
					board->update(board->timestep);
				}
			}
		}
	}

	board->update(elapsedTime);
}

static void render() {
	window->clear(sf::Color::Black);
	board->render();
	window->display();
}