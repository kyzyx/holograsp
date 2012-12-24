#pragma once
#include "inputmanager.h"
#include "util_pipeline.h"
#include "HandData.h"

class UtilPipelineInputManager :
	public InputManager
{
public:
	UtilPipelineInputManager(void);
	~UtilPipelineInputManager(void);

	virtual bool init();
	virtual void update();
	virtual float* getDepthFrame();
	virtual float* getRGBFrame();
	virtual float* getCoords();
	virtual void getDepthFrame(void* ptr);
	virtual void getRGBFrame(void* ptr);
	virtual void getCoords(void* ptr);
	virtual HandData getSkelData();

protected:
	UtilPipeline pipeline;

	unsigned int wi, hi;
	PXCGesture::GeoNode::Label* bodyLabels;
	int* labeltodatamap;

	/* Raw SDK output variables */
	float* uv;
	short* coords;
	unsigned char* rgb;
	short* depth;
	HandData handdata;

	/* Standardized variables */
	float* out_rgb;
	float* out_coords;
	float* out_depth;
};

