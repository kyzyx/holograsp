#include "MeshCtlHandler.h"

const double MeshCtlHandler::THRESHOLD = 0.002;

MeshCtlHandler::MeshCtlHandler(App* myapp, Mesh* m, Mesh* c)
{
	app = myapp;
	mesh = m;
	cursor= c;
	move = 0;
	speed = 0.002f;
	wheelspeed = 0.04f;
	precisespeed = 0.01f;
	mousezspeed = 0.0005f;
	precision = false;
	selected = false;
	zaxis = false;
	lastx = -1;
	lasty = -1;
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_GrabInput(SDL_GRAB_ON);
}


MeshCtlHandler::~MeshCtlHandler(void)
{
}

void MeshCtlHandler::update() {
	float trans[3];
	trans[0] = 0; trans[1] = 0; trans[2] = 0;
	if (move & M_UP) {
		trans[1] += speed;
	}
	if (move & M_DOWN) {
		trans[1] -= speed;
	}
	if (move & M_RIGHT) {
		trans[0] += speed;
	}
	if (move & M_LEFT) {
		trans[0] -= speed;
	}
	if (move & M_FORWARD) {
		trans[2] -= speed;
	}
	if (move & M_BACKWARD) {
		trans[2] += speed;
	}
	mesh->translateBy(trans);
	if (selected) {
		mesh->setColor(0.f,1.f,0.f);
	} else if (dist2(mesh->getTranslation(), cursor->getTranslation()) < THRESHOLD) {
		mesh->setColor(0.f,0.f,1.f);
	} else {
		mesh->setColor(1.f,0.f,0.f);
	}
	cursor->setColor(0.f,1.f,0.f);
}

void MeshCtlHandler::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) {
        case SDLK_w:
            move |= M_FORWARD;
            break;
        case SDLK_a:
            move |= M_LEFT;
            break;
        case SDLK_s:
            move |= M_BACKWARD;
            break;
        case SDLK_d:
            move |= M_RIGHT;
            break;
        case SDLK_e:
            move |= M_UP;
            break;
        case SDLK_c:
            move |= M_DOWN;
            break;
		case SDLK_SPACE:
			zaxis = true;
			break;
        case SDLK_ESCAPE:
        case SDLK_q:
            app->stop();
        default:
            break;
	}
}

void MeshCtlHandler::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) {
        case SDLK_w:
            move &= ~M_FORWARD;
            break;
        case SDLK_a:
            move &= ~M_LEFT;
            break;
        case SDLK_s:
            move &= ~M_BACKWARD;
            break;
        case SDLK_d:
            move &= ~M_RIGHT;
            break;
        case SDLK_e:
            move &= ~M_UP;
            break;
        case SDLK_c:
            move &= ~M_DOWN;
            break;
		case SDLK_SPACE:
			zaxis = false;
			break;
        default:
            break;
	}
}

// Returns qxprop scaled into the appropriate width
inline float scale(float z1, float x1, float z2, float x2, float qz, float qxprop) {
	return ((qz-z1)*(x2-x1)/(z2-z1) + x1)*qxprop;
}

void MeshCtlHandler::OnMouseMove(int mX, int mY, int relX, int relY, bool Left,bool Right,bool Middle) {
	if (lastx == -1) {
		lastx = mX; lasty = app->getHeight() - mY - 1;
	}
	
	int nmX = lastx + relX;
	int nmY = lasty - relY;
	if (nmX > app->getWidth()) nmX = app->getWidth();
	else if (nmX < 0) nmX = 0;
	if (nmY > app->getHeight()) nmY = app->getHeight();
	else if (nmY < 0) nmY = 0;
	lastx = nmX;

	// Scale x, y according to screen space
	float dx = 0.f; float dy = 0.f; float dz = 0.f;
	if (!(app->getWidth() & 1) && nmX >= app->getWidth()/2) ++nmX;
	if (!(app->getHeight() & 1) && nmY >= app->getHeight()/2) ++nmY;
	float xx = (nmX - app->getWidth()/2)/(float)(app->getWidth()/2);
	const float z = cursor->getTranslation()[2];
	if (zaxis) {
		float sp = relY*mousezspeed;
		if (z + sp > -0.02f) sp = -0.02f - z;
		float x = scale(-0.02f, 0.198f, -1.34f, 0.706f, z+sp, xx);
		float yy = cursor->getTranslation()[1];
		float ya = 1./scale(-0.02f, 0.140f, -1.34f, 0.410f, z, 1.f/yy);
		float y = scale(-0.02f, 0.140f, -1.34f, 0.410f, z+sp, ya);
		dx = x - cursor->getTranslation()[0];
		dy = y - cursor->getTranslation()[1];
		dz = sp;
	} else {
		float yy = (nmY - app->getHeight()/2)/(float)(app->getHeight()/2);
		// USING MAGIC NUMBERS
		float x = scale(-0.02f, 0.198f, -1.34f, 0.706f, z, xx);
		float y = scale(-0.02f, 0.140f, -1.34f, 0.410f, z, yy);
		dx = x - cursor->getTranslation()[0];
		dy = y - cursor->getTranslation()[1];
		lasty = nmY;
	}
	cursor->translateBy(dx, dy, dz);
	if (selected) mesh->translateBy(dx, dy, dz);
}

void MeshCtlHandler::moveZ(float n) {
	float sp = precision?precisespeed:wheelspeed;
	sp *= n;
	float z = cursor->getTranslation()[2];
	if (z + sp > -0.02) return;
	float xx = cursor->getTranslation()[0];
	float yy = cursor->getTranslation()[1];
	float xa = 1./scale(-0.02f, 0.198f, -1.34f, 0.706f, z, 1.f/xx);
	float ya = 1./scale(-0.02f, 0.140f, -1.34f, 0.410f, z, 1.f/yy);
	float x = scale(-0.02f, 0.198f, -1.34f, 0.706f, z+sp, xa);
	float y = scale(-0.02f, 0.140f, -1.34f, 0.410f, z+sp, ya);
	float dx = x - cursor->getTranslation()[0];
	float dy = y - cursor->getTranslation()[1];
	cursor->translateBy(dx,dy,sp);
	if (selected) mesh->translateBy(dx,dy,sp);
}
/*
void MeshCtlHandler::OnMouseWheel(bool Up, bool Down) {
	moveZ(Up?-1:1);
	
}
void MeshCtlHandler::OnMButtonDown(int mX, int mY) {
	precision = true;
}
void MeshCtlHandler::OnMButtonUp(int mX, int mY) {
	precision = false;
}
*/

void MeshCtlHandler::OnLButtonDown(int mX, int mY) {
	if (dist2(mesh->getTranslation(), cursor->getTranslation()) < THRESHOLD) {
		selected = true;
	}
}
void MeshCtlHandler::OnLButtonUp(int mX, int mY) {
	selected = false;
}