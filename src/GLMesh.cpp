#include "GLMesh.h"
#include <gl\glew.h>
#include <cstdlib>

GLMesh::GLMesh(void)
{

}

GLMesh::GLMesh(const char* filename, Renderer* r, bool reverse) : Mesh(filename, r, reverse) {
	glewInit();
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, 3*nvert*sizeof(float), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nface*3*sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	if (use_color) {
		glGenBuffers(1, &cbuf);
		glBindBuffer(GL_ARRAY_BUFFER, cbuf);
		glBufferData(GL_ARRAY_BUFFER, 3*nvert*sizeof(float), colors, GL_STATIC_DRAW);
	}
}

GLMesh::~GLMesh(void)
{
}

void GLMesh::draw() {
	render->predraw();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	render->applyTranslation(translation[0], translation[1], translation[2]);
	render->applyRotation(rotation[0], rotation[1], rotation[2]);
	render->applyScale(scale[0], scale[1], scale[2]);

	glColor3f(1.,1.,1.);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	if (use_color) {
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, cbuf);
		glColorPointer(4, GL_FLOAT, 0, NULL);
	}
	glDrawElements(GL_TRIANGLES, nface*3, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);
	if (use_color) glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
}

void GLMesh::drawWireframe() {
	render->predraw(false);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	render->applyTranslation(translation[0], translation[1], translation[2]);
	render->applyRotation(rotation[0], rotation[1], rotation[2]);
	render->applyScale(scale[0], scale[1], scale[2]);
	float tmp[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, tmp);
	glDisable(GL_LIGHTING);
	glColor3f(1.,0,0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	if (use_color) {
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, cbuf);
		glColorPointer(4, GL_FLOAT, 0, NULL);
	}
	glDrawElements(GL_TRIANGLES, nface*3, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);
	if (use_color) glDisableClientState(GL_COLOR_ARRAY);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}


void GLMesh::writeBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, 3*nvert*sizeof(float), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nface*3*sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	if (use_color) {
		glBindBuffer(GL_ARRAY_BUFFER, cbuf);
		glBufferData(GL_ARRAY_BUFFER, 3*nvert*sizeof(float), colors, GL_STATIC_DRAW);
	}
}