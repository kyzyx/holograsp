#pragma once
#include "gesturectlhandler.h"
#include "Filter.h"

class ClosestPointCtlHandler :
	public GestureCtlHandler
{
public:
	ClosestPointCtlHandler(App* myapp, Mesh* m, Mesh* c, InputManager* mgr);
	~ClosestPointCtlHandler(void);

	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual int getState() {
		if (selected) return GestureCtlHandler::ENGAGED;
		else return GestureCtlHandler::TRACKING;
	}
protected:
	virtual void updateCursorPosition();

	Filter* coordfilters[3];
	bool visualize;
};

