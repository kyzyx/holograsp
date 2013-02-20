#pragma once
#include "directxapp.h"
#define OPENGL_RENDERER
#include "SpeedTestController.h"
#include "Visualizer.h"
#include "UtilPipelineInputManager.h"

class VisualizationApp :
	public DirectXApp
{
public:
	VisualizationApp(void);
	~VisualizationApp(void);

	virtual bool onInit();
	virtual void onLoop();
	virtual void onRender();

private:
	SpeedTestController* sc;
	Visualizer* vis;
	UtilPipelineInputManager* manager;
};

