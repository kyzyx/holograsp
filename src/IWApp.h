#pragma once
#include "GLApp.h"
#include "Visualizer.h"
#include "InputManager.h"
#include "HandController.h"

class IWApp : public GLApp {
public:
	IWApp(void);
	~IWApp(void);

	virtual bool onInit();
	virtual void onLoop();
	virtual void onRender();
		
protected:
	InputManager* manager;
	Visualizer vis;
	HandController* ctl;
};

