#pragma once
#include "cameractlhandler.h"
class D3DCameraCtl :
	public CameraCtlHandler
{
public:
	D3DCameraCtl(void) {;}
	D3DCameraCtl(App* myapp, int width, int height, int yfov) : CameraCtlHandler(myapp, width, height, yfov) { setCamera(); }
	~D3DCameraCtl(void) {;}

	virtual void setCamera();
};

