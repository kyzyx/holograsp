#pragma once
#include "HandData.h"

class InputManager
{
public:
	InputManager(void) {;}
	~InputManager(void) {;}

	virtual bool init() = 0;
	virtual void update() = 0;
	virtual float* getDepthFrame() = 0;
	virtual float* getRGBFrame() = 0;
	virtual float* getCoords() = 0;
	virtual void getDepthFrame(void* ptr) = 0;
	virtual void getRGBFrame(void* ptr) = 0;
	virtual void getCoords(void* ptr) = 0;
	virtual HandData getSkelData() = 0;

	virtual int width() const { return w; }
	virtual int height() const { return h; }
protected:
	unsigned int w,h;
};

