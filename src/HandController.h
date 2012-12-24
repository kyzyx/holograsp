#pragma once
#include "modelcontroller.h"
#include "HandData.h"
#include "InputManager.h"

class HandController :
	public ModelController
{
public:
	HandController(void);
	~HandController(void);

	virtual bool init(InputManager* m, Renderer* r);
	virtual void debug();
	virtual void update();
	virtual void draw() {
		for (unsigned int i = 0; i < meshes.size(); ++i) {
			meshes[i]->setColor(1.f,0.f,0.f);
			meshes[i]->drawWireframe();
		}
	}

private:
	void closestPoint();
	InputManager* manager;
	Renderer* render;
	enum {
		NONE,      // No hand sensed
		NEUTRAL,   // Hand not tracking
		TRACKING,  // Hand tracking (2 fingers separated)
		ENGAGED,   // Pinched fingers
	};
	int state[2];
	int newstate[2];
	int counter[2];
	int threshold;

	float hand[2][3];

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
};

