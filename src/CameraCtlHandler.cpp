#include "CameraCtlHandler.h"
#include <cmath>

CameraCtlHandler::CameraCtlHandler(void)
{
}

static inline void cross(float* a, float* b, float* c) {
	c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}
static inline void normalize(float* a) {
	float tot = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	a[0] /= tot;
	a[1] /= tot;
	a[2] /= tot;
}

CameraCtlHandler::CameraCtlHandler(App* myapp, Renderer* r, int width, int height, int yfov) {
	app = myapp;
	render = r;
	w = width;
	h = height;
	p = (float) yfov;
	move = 0;
	translateSpeed = 0.01f;
	rotateSpeed = 0.01f;

	for (int i = 0; i < 3; ++i) {
		eye[i] = 0;
		towards[i] = 0;
		up[i] = 0;
	}
	up[1] = 1;
	towards[2] = 1;
	cross(towards, up, right);
	normalize(right);
}

CameraCtlHandler::~CameraCtlHandler(void)
{
}

void CameraCtlHandler::update() {
    if (move & TRANS_UP) {
		for (int i = 0; i < 3; ++i) eye[i] += up[i]*translateSpeed;
	}
	if (move & TRANS_DOWN) {
		for (int i = 0; i < 3; ++i) eye[i] -= up[i]*translateSpeed;
	}
	if (move & TRANS_RIGHT) {
		for (int i = 0; i < 3; ++i) eye[i] += right[i]*translateSpeed;
	}
	if (move & TRANS_LEFT) {
		for (int i = 0; i < 3; ++i) eye[i] -= right[i]*translateSpeed;
	}
	if (move & TRANS_FORWARD) {
		for (int i = 0; i < 3; ++i) eye[i] += towards[i]*translateSpeed;
	}
	if (move & TRANS_BACKWARD) {
		for (int i = 0; i < 3; ++i) eye[i] -= towards[i]*translateSpeed;
	}
	if (move & ROT_UP) {
        for (int i = 0; i < 3; ++i) towards[i] = towards[i]*cos(rotateSpeed) + up[i]*sin(rotateSpeed);
		normalize(towards);
        cross(right, towards, up);
		normalize(up);
    }
    if (move & ROT_DOWN) {
        for (int i = 0; i < 3; ++i) towards[i] = towards[i]*cos(rotateSpeed) - up[i]*sin(rotateSpeed);
        normalize(towards);
		cross(right, towards, up);
		normalize(up);
    }
    if (move & ROT_LEFT) {
        for (int i = 0; i < 3; ++i) towards[i] = towards[i]*cos(rotateSpeed) - right[i]*sin(rotateSpeed);
		normalize(towards);
		cross(towards, up, right);
		normalize(right);
    }
    if (move & ROT_RIGHT) {
        for (int i = 0; i < 3; ++i) towards[i] = towards[i]*cos(rotateSpeed) + right[i]*sin(rotateSpeed);
		normalize(towards);
		cross(towards, up, right);
		normalize(right);
    }
	render->setProjection(p, w / (float) h);
	render->lookAt(eye, towards, up);
}

void CameraCtlHandler::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) {
		case SDLK_LEFT:
			move |= ROT_LEFT;
			break;
		case SDLK_RIGHT:
			move |= ROT_RIGHT;
			break;
		case SDLK_UP:
			move |= ROT_UP;
			break;
		case SDLK_DOWN:
			move |= ROT_DOWN;
			break;
        case SDLK_w:
            move |= TRANS_FORWARD;
            break;
        case SDLK_a:
            move |= TRANS_LEFT;
            break;
        case SDLK_s:
            move |= TRANS_BACKWARD;
            break;
        case SDLK_d:
            move |= TRANS_RIGHT;
            break;
        case SDLK_e:
            move |= TRANS_UP;
            break;
        case SDLK_c:
            move |= TRANS_DOWN;
            break;
        case SDLK_ESCAPE:
        case SDLK_q:
            app->stop();
        default:
            break;
	}
}

void CameraCtlHandler::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) {
		case SDLK_LEFT:
			move &= ~ROT_LEFT;
			break;
		case SDLK_RIGHT:
			move &= ~ROT_RIGHT;
			break;
		case SDLK_UP:
			move &= ~ROT_UP;
			break;
		case SDLK_DOWN:
			move &= ~ROT_DOWN;
			break;
        case SDLK_w:
            move &= ~TRANS_FORWARD;
            break;
        case SDLK_a:
            move &= ~TRANS_LEFT;
            break;
        case SDLK_s:
            move &= ~TRANS_BACKWARD;
            break;
        case SDLK_d:
            move &= ~TRANS_RIGHT;
            break;
        case SDLK_e:
            move &= ~TRANS_UP;
            break;
        case SDLK_c:
            move &= ~TRANS_DOWN;
            break;
        default:
            break;
	}
}