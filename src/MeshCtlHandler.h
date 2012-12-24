#pragma once
#include "sdlhandler.h"
#include "Mesh.h"

class MeshCtlHandler :
	public SDLHandler
{
public:
	MeshCtlHandler(App* myapp, Mesh* m, Mesh* c);
	~MeshCtlHandler(void);

	virtual void update();
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	virtual void OnMouseMove(int mX, int mY, int relX, int relY, bool Left,bool Right,bool Middle);
	virtual void OnLButtonDown(int mX, int mY);
	virtual void OnLButtonUp(int mX, int mY);
	
	/*virtual void OnMouseWheel(bool Up, bool Down);
	virtual void OnMButtonDown(int mX, int mY);
	virtual void OnMButtonUp(int mX, int mY);*/

	bool isSelected() const { return selected; }
private:
	virtual void moveZ(float n);
	enum {
		M_UP = 1,
		M_DOWN = 2,
		M_RIGHT = 4,
		M_LEFT = 8,
		M_FORWARD = 16,
		M_BACKWARD = 32,
	};
	static const double THRESHOLD;
	float speed;
	float wheelspeed;
	float precisespeed;
	float mousezspeed;
	int move;
	Mesh* mesh;
	Mesh* cursor;
	bool selected;
	bool precision;
	bool zaxis;
	int lastx;
	int lasty;
};

