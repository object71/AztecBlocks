#include "Game.h"
#include "GameLoop.h"
#include "GameBoard.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std::chrono;

Game::Game() : 
	m_window(sf::VideoMode(640, 480), "Jungle Blocks", sf::Style::Titlebar | sf::Style::Close),
	m_board(m_window, 5, 9)
{
}

Game::~Game() {
	m_window.close();
}

void Game::update(milliseconds elapsedTime) {
	m_board.update(elapsedTime);
}

void Game::draw(std::chrono::milliseconds elapsedTime) {
	m_window.clear(sf::Color::Black);
	m_board.draw(elapsedTime);
	m_window.display();
}