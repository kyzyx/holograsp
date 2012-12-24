#include "GLRenderer.h"
#include <cstring>
#include <gl\glew.h>
#include <SDL.h>

GLRenderer::GLRenderer(void) {
	glewInit();
	glGenBuffers(NUM_BUFFERS, ids);
	for (int i = 0; i < NUM_BUFFERS; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, ids[i]);
	}
	transformed = false;
}

void GLRenderer::predraw(bool lights)  {
	if (lights) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
}
void GLRenderer::drawPoints(int n, float* coords, float* colors, float pointSize) {
	GLubyte* ptr;
	glDisable(GL_LIGHTING);
	glBindBuffer(GL_ARRAY_BUFFER, ids[0]);
	glBufferData(GL_ARRAY_BUFFER, n*3*sizeof(float), 0, GL_DYNAMIC_DRAW);
	ptr = (GLubyte*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		memcpy(ptr, coords, n*3*sizeof(float));
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	if (colors) {
		glBindBuffer(GL_ARRAY_BUFFER, ids[1]);
		glBufferData(GL_ARRAY_BUFFER, n*3*sizeof(float), 0, GL_DYNAMIC_DRAW);
		ptr = (GLubyte*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if (ptr) {
			memcpy(ptr, colors, n*3*sizeof(float));
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	if (colors) glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, ids[0]);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	
	if (colors) {
		glBindBuffer(GL_ARRAY_BUFFER, ids[1]);
		glColorPointer(3, GL_FLOAT, 0, NULL);
	} else {
		glColor3f(1.,1.,1.);
	}
	glPointSize(pointSize);
	glDrawArrays(GL_POINTS, 0, n);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	if (colors) glDisableClientState(GL_COLOR_ARRAY);
	glEnable(GL_LIGHTING);
}

void GLRenderer::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void GLRenderer::display() {
	    SDL_GL_SwapBuffers();
}
void GLRenderer::setProjection(float fov, float aspect, float nearclip, float farclip) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, nearclip, farclip);

}
void GLRenderer::lookAt(float eye[3], float towards[3], float up[3]) {
  // Set camera transformation
  //GLdouble camera_matrix[16] = { right[0], up[0], -towards[0], 0, right[1], up[1], -towards[1], 0, right[2], up[2], -towards[2], 0, 0, 0, 0, 1 };
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye[0], eye[1], eye[2], eye[0]+towards[0], eye[1]+towards[1], eye[2]+towards[2], up[0], up[1], up[2]);
  /*glMultMatrixd(camera_matrix);
  glTranslated(-eye[0], -eye[1], -eye[2]);*/
}

void GLRenderer::ortho() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
}

void GLRenderer::ortho(float ww, float hh) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(-ww/2, ww/2, -hh/2, hh/2);
}

void GLRenderer::applyRotation(float a, float b, float c) {
	glMatrixMode(GL_MODELVIEW);
	glRotatef(a,1,0,0);
	glRotatef(b,0,1,0);
	glRotatef(c,0,0,1);
	transformed = true;
}
void GLRenderer::applyTranslation(float a, float b, float c) {
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(a,b,c);
	transformed = true;
}
void GLRenderer::applyScale(float a, float b, float c) {
	glMatrixMode(GL_MODELVIEW);
	glScalef(a,b,c);
	transformed = true;
}
void GLRenderer::resetTransforms() {
	transformed = false;
}