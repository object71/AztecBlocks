#pragma once

#include <chrono>

class IUpdateable {
public:
	virtual void update(std::chrono::milliseconds elapsedTime) = 0;
};

class IDrawable {
public:
	virtual void draw(std::chrono::milliseconds elapsedTime) = 0;
};

class IFullComponent : public IDrawable, public IUpdateable {

};