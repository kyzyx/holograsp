#pragma once
#include "sdlhandler.h"
class StatusScreenHandler :
	public SDLHandler
{
public:
	StatusScreenHandler(void) : num_clicks(0) {;}
	StatusScreenHandler(App* myapp)  : num_clicks(0), SDLHandler(myapp) {;}
	~StatusScreenHandler(void) {;}
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
		switch(sym) {
			case SDLK_SPACE: ++num_clicks;
				break;
			case SDLK_ESCAPE:
				app->stop();
				break;
			default:
				break;
		}
	}
	virtual void OnLButtonDown(int mX, int mY) {
		++num_clicks;
	}
	virtual int status() {
		return num_clicks;
	}
protected:
	int num_clicks;
};

