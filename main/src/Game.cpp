#include <thread>
#include <cmrc/cmrc.hpp>
#include "Game.h"
#include "GameLoop.h"
#include "GameBoard.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std::chrono;

void run();

int main(int argc, char* argv[]) {

	std::thread gameLoop(&run);
	gameLoop.join();

	return 0;
}

static void run() {
	IFullComponent& game = Game();
	GameLoop loop(game);

	loop.run();
}

Game::Game() : 
	m_window(sf::VideoMode(640, 480), "Jungle Blocks", sf::Style::Titlebar | sf::Style::Close),
	m_board(m_window, 5, 9)
{
}

Game::~Game() {
	m_window.close();
}

void Game::update(milliseconds elapsedTime) {

	if (m_window.isOpen()) {
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				// TODO: Find how to close properly
				//setRunning(false);
			}
			else if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::Left) {
					m_board.move(Direction::Left);
				}
				else if (event.key.code == sf::Keyboard::Right) {
					m_board.move(Direction::Right);
				}
				else if (event.key.code == sf::Keyboard::Space) {
					if (m_board.state == State::GameOver || m_board.state == Initial) {
						m_board.state = State::Restart;
					}
					else {
						m_board.rotate();
					}
				}
				else if (event.key.code == sf::Keyboard::Down) {
					m_board.update(m_board.timestep);
				}
				else if (event.key.code == sf::Keyboard::M) {
					m_board.setMute(m_board.getMute() ? false : true);
				}
				else if (event.key.code == sf::Keyboard::Escape) {
					if (m_board.state != State::Paused) {
						m_board.state = State::Paused;
					}
					else {
						m_board.state = State::BlockFalling;
					}
				}
			}
		}
	}

	m_board.update(elapsedTime);
}

void Game::draw(std::chrono::milliseconds elapsedTime) {
	m_window.clear(sf::Color::Black);
	m_board.draw(elapsedTime);
	m_window.display();
}