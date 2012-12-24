#pragma once
#include "renderer.h"
#include <Windows.h>

class GLRenderer :
	public Renderer
{
public:
	GLRenderer(void);
	~GLRenderer(void) {;}
	
	virtual void drawPoints(int n, float* coords, float* colors, float pointSize);
	virtual void drawText(const WCHAR* text, float x, float y, int color, float size) {;}
	virtual void predraw(bool lights=true);
	virtual void clear();
	virtual void display();

	virtual void setProjection(float fov, float aspect, float near, float far);
	virtual void lookAt(float eye[3], float towards[3], float up[3]);
	virtual void ortho();
	virtual void ortho(float w, float h);

	virtual void applyRotation(float a, float b, float c);
	virtual void applyTranslation(float a, float b, float c);
	virtual void applyScale(float a, float b, float c);
	virtual void resetTransforms();
private:
	static const int NUM_BUFFERS = 4;
	unsigned int ids[NUM_BUFFERS];
	bool transformed;
};