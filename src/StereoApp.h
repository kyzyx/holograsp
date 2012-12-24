#pragma once
#include "directxapp.h"
#include "Visualizer.h"
#include "InputManager.h"
#include "HandController.h"

class StereoApp :
	public DirectXApp
{
public:
	StereoApp(void);
	~StereoApp(void);

	virtual bool onInit();
	virtual void onLoop();
	virtual void onRender();

private:
	Mesh* mesh;
	InputManager* manager;
	Visualizer vis;
	HandController* ctl;
};

