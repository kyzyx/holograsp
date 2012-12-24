#pragma once
#include <Windows.h>
class Renderer
{
public:
	Renderer(void) { for (int i = 0; i < 3; ++i) clearcolor[i] = 0.f; clearcolor[3] = 1.f; }
	~Renderer(void) { ; }

	// Drawing functions
	virtual void drawPoints(int n, float* coords, float* colors, float pointSize=1.f) = 0;
	virtual void drawText(const WCHAR* text, float x, float y, int color, float size) = 0;
	virtual void predraw(bool lights=true) = 0;
	virtual void clear() = 0;
	virtual void setClearColor(float r, float g, float b) {
		clearcolor[0] = r; clearcolor[1] = g; clearcolor[2] = b;
	}
	virtual void setClearColor(const float* c) {
		for (int i = 0; i < 3; ++i) clearcolor[i] = c[i];
	}
	virtual void display() = 0;

	// Camera Matrix Functions
	virtual void setProjection(float fov, float aspect, float near=0.1, float far=1000.) = 0;
	virtual void lookAt(float eye[3], float towards[3], float up[3]) = 0;
	virtual void ortho() = 0;
	virtual void ortho(float w, float h) = 0;

	// Object Matrix Functions
	virtual void applyRotation(float a, float b, float c) = 0;
	virtual void applyTranslation(float a, float b, float c) = 0;
	virtual void applyScale(float a, float b, float c) = 0;
	virtual void resetTransforms() = 0;
protected:
	float clearcolor[4];
};

