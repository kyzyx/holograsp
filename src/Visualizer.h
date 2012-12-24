#pragma once
#include "InputManager.h"
#include "Renderer.h"

class Visualizer
{
public:
	Visualizer(void);
	~Visualizer(void);

	virtual void init(InputManager* m, Renderer* r);
	virtual void draw();

protected:
	InputManager* manager;
	Renderer* render;
};

