#include "Game.h"
#include "GameLoop.h"
#include <thread>

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