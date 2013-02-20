#pragma once
#include "sdlhandler.h"
#include "InputManager.h"
#include "Mesh.h"

class GestureCtlHandler :
	public SDLHandler
{
public:
	GestureCtlHandler(App* myapp, Mesh* m, Mesh* c, InputManager* mgr);
	~GestureCtlHandler(void);

	virtual void update();
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	int getState() { return state[0]; }
	enum {
		NONE,      // No hand sensed
		NEUTRAL,   // Hand not tracking
		TRACKING,  // Hand tracking (2 fingers separated)
		ENGAGED,   // Pinched fingers
	};
protected:
	void GestureCoordsToWorldCoords(const float* src, float* dst);
	void WorldCoordsToScreenCoords(const float* src, float* dst);

	virtual void updateCursorPosition();

	void updateHand(int n, const HandData& h);
	bool hysteresis(int n, int prop) {
		// Want to go from state[n] to prop
		// TODO: allow some number of mistakes
		// If what I'm changing to doesn't match the proposed state, reset the counter and attempted 
		if (prop != newstate[n]) {
			counter[n] = 0;
			if (prop != state[n]) newstate[n] = prop;
		} else {
			// If what I'm changing to does match the state, change state if counter is high enough
			if (counter[n] == threshold) {
				state[n] = prop;
				newstate[n] = prop;
				counter[n] = 0;
				return true;
			} else {
				// Otherwise increment the counter
				++counter[n];
			}
		}
		return false;
	}
	
	Mesh* mesh;
	Mesh* cursor;
	InputManager* manager;
	bool selected;
	static const double THRESHOLD;

	int state[2];
	int newstate[2];
	int counter[2];
	int threshold;

	float delta[3];
	float hand[2][3];

	float fingertips[3];
	float offscreen[3];
	float correction[3];
	float linear[3];
};

