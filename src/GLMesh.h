#pragma once
#include "mesh.h"
class GLMesh :
	public Mesh
{
public:
	GLMesh(void);
	GLMesh(const char* filename, Renderer* r, bool reverse=false);
	~GLMesh(void);

	virtual void draw();
	virtual void drawWireframe();
	virtual void writeBuffer();

private:
	unsigned int vbuf, cbuf, ibuf;
};

