#pragma once
#include "Renderer.h"
#include <exception>
#include <string>

class OFFFileFormatError : public std::exception {
public:
	OFFFileFormatError(std::string s){
		err = s;
	}
	std::string err;
};

class Mesh
{
public:
	Mesh(void);
	Mesh(const char* filename, Renderer* r, bool reverse=false);
	~Mesh(void);

	virtual void draw() = 0;
	virtual void drawWireframe() = 0;
	virtual void writeBuffer() = 0;
	bool ReadOff(const char* filename, bool reverse=false);

	const float* getTranslation() { return translation; }
	const float* getRotation() { return rotation; }
	const float* getScale() { return scale; }
	
	void setColor(float x, float y, float z, float a=1.0f) {
		for (int i = 0; i < nvert; ++i) {
			colors[4*i] = x; colors[4*i+1] = y; colors[4*i+2] = z; colors[4*i+3] = a;
		}
		writeBuffer();
	}
	const float* getColor() {
		return colors;
	}

	void translateBy(float x, float y, float z) {
		translation[0] += x; translation[1] += y; translation[2] += z;
	}
	void translateBy(const float* x) {
		translation[0] += x[0]; translation[1] += x[1]; translation[2] += x[2];
	}
	void setTranslation(float x, float y, float z) {
		translation[0] = x; translation[1] = y; translation[2] = z;
	}
	void setTranslation(const float* x) {
		translation[0] = x[0]; translation[1] = x[1]; translation[2] = x[2];
	}
	void rotateBy(float x, float y, float z) {
		rotation[0] += x; rotation[1] += y; rotation[2] += z;
	}
	void rotateBy(const float* x) {
		rotation[0] += x[0]; rotation[1] += x[1]; rotation[2] += x[2];
	}
	void setRotation(float x, float y, float z) {
		rotation[0] = x; rotation[1] = y; rotation[2] = z;
	}
	void setRotation(const float* x) {
		rotation[0] = x[0]; rotation[1] = x[1]; rotation[2] = x[2];
	}
	void scaleBy(float x, float y, float z) {
		scale[0] *= x; scale[1] *= y; scale[2] *= z;
	}
	void scaleBy(const float* x) {
		scale[0] *= x[0]; scale[1] *= x[1]; scale[2] *= x[2];
	}
	void setScale(float x, float y, float z) {
		scale[0] = x; scale[1] = y; scale[2] = z;
	}
	void setScale(const float* x) {
		scale[0] = x[0]; scale[1] = x[1]; scale[2] = x[2];
	}
	void setScale(float x) {
		scale[0] = x; scale[1] = x; scale[2] = x;
	}
	void scaleBy(float x) {
		scale[0] *= x; scale[1] *= x; scale[2] *= x;
	}
protected:
	void calcNormals();

	Renderer* render;

	float* vertices;
	float* colors;
	float* normals;
	float* texture;
	unsigned int* indices;

	int nvert, nface, nedge;
	bool use_color;

	float translation[3];
	float rotation[3];
	float scale[3];
};


inline float dist2(const float a[3], const float b[3]) {
	return (a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
}

inline void cross(const float* a, const float* b, float* c) {
	c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}

inline void normalize(float* a) {
	float tot = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	a[0] /= tot;
	a[1] /= tot;
	a[2] /= tot;
}