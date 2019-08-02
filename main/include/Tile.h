#pragma once

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
