#pragma once
#include "app.h"
#include "Renderer.h"

class GLApp :
	public App
{
public:
	GLApp(void);
	~GLApp(void);

	virtual bool onInit();
	virtual void onRender();

protected:
	Renderer* render;
};

