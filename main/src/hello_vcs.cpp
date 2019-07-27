#include <cmrc/cmrc.hpp>
#include "Common.h"
#include <thread>
#include "GameLoop.h"
#include "GameBoard.h"

using namespace std::chrono;

void update(milliseconds elapsedTime);
void render(milliseconds elapsedTime);
void run();

GameLoop* game;
GameBoard* board;
sf::RenderWindow* window;

int main(int argc, char* argv[]) {

	std::thread gameLoop(&run);
	gameLoop.join();

	return 0;
}

static void run() {
	window = new sf::RenderWindow(sf::VideoMode(640, 480), "Jungle Blocks", sf::Style::Titlebar | sf::Style::Close);

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
			if (event.type == sf::Event::Closed) {
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
					if (board->state == State::GameOver || board->state == Initial) {
						board->state = State::Restart;
					}
					else {
						board->rotate();
					}
				}
				else if (event.key.code == sf::Keyboard::Down) {
					board->update(board->timestep);
				}
				else if (event.key.code == sf::Keyboard::M) {
					board->setMute(board->getMute() ? false : true);
				}
				else if (event.key.code == sf::Keyboard::Escape) {
					if (board->state != State::Paused) {
						board->state = State::Paused;
					}
					else {
						board->state = State::BlockFalling;
					}
				}
			}
		}
	}

	board->update(elapsedTime);
}

static void render(std::chrono::milliseconds elapsedTime) {
	window->clear(sf::Color::Black);
	board->render(elapsedTime);
	window->display();
}