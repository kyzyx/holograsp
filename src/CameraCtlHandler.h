#pragma once
#include <SDL.h>
#include "sdlhandler.h"
#include "Renderer.h"

class CameraCtlHandler :
	public SDLHandler
{
public:
	CameraCtlHandler(void);
	CameraCtlHandler(App* myapp, Renderer* r, int width, int height, int yfov);
	~CameraCtlHandler(void);

	virtual void update();
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

protected:
	enum {
        TRANS_UP    = 1,
        TRANS_DOWN  = 2,
        TRANS_LEFT  = 4,
        TRANS_RIGHT = 8,
        TRANS_FORWARD  = 16,
        TRANS_BACKWARD = 32,
        ROT_UP    = 64,
        ROT_DOWN  = 128,
        ROT_LEFT  = 256,
        ROT_RIGHT = 512
    };

	float translateSpeed;
	float rotateSpeed;
	App* app;
	Renderer* render;

	int move;

	int w;
	int h;
	float p;
	float eye[3];
	float towards[3];
	float up[3];
	float right[3];
};

