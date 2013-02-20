#include "GestureCtlHandler.h"

const double GestureCtlHandler::THRESHOLD = 0.002;

GestureCtlHandler::GestureCtlHandler(App* myapp, Mesh* m, Mesh* c, InputManager* mgr)
{
	app = myapp;
	mesh = m;
	cursor= c;
	manager = mgr;
	selected = false;
	SDL_ShowCursor(SDL_DISABLE);

	state[0] = NONE;
	state[1] = NONE;
	newstate[0] = NONE;
	newstate[1] = NONE;
	counter[0] = 0;
	counter[1] = 0;
	threshold = 5;

	offscreen[0] = 0;
	offscreen[1] = 0;
	offscreen[2] = 1000;

	correction[0] = 0;
	correction[1] = 15;
	correction[2] = 110;  //650 for visualization 
	linear[0] = 0.8f;
	linear[1] = 1.f;
	linear[2] = 1.1f; // 0.3
	cursor->setTranslation(offscreen);
	// 0 15 110
	// 0.8 1 1
}


GestureCtlHandler::~GestureCtlHandler(void)
{
}

// Two fingers, close together
// Two fingers, noisy
// More fingers, close to center?
// Finger confidences?
static int countFingers(const HandData& h, int hand) {
	int nf = 0;
	
	static const int lfingers[] = {HandData::LEFT_INDEX, HandData::LEFT_RING, HandData::LEFT_MIDDLE, HandData::LEFT_PINKY, HandData::LEFT_THUMB};
	static const int rfingers[] = {HandData::RIGHT_INDEX, HandData::RIGHT_RING, HandData::RIGHT_MIDDLE, HandData::RIGHT_PINKY, HandData::RIGHT_THUMB};
	float coords[5][3];

	/*float handcenter[3]; 
	h.getData(hand, handcenter);
	static const float distancethreshold = 0.02*0.02;*/

	for (int i = 0; i < 5; ++i) {
		bool unique = false;
		if (hand == HandData::LEFT_HAND && h.getData(lfingers[i], coords[i])) {
			unique = true;	
		}
		if (hand == HandData::RIGHT_HAND && h.getData(rfingers[i], coords[i])) {
			unique = true;
		}
		if (!unique) continue;

		// If unique and far enough from hand center
		for (int j = 0; j < i; ++j) {
			if (coords[j][0] == coords[i][0] && coords[j][1] == coords[i][1] && coords[j][2] == coords[i][2]) {
				unique = false;
				break;
			}
		}
		if (unique) ++nf;
	}
	return nf;
}

static void getFingertips(const HandData& h, int hand, float dst[3]) {
	int nf = 0;
	static const int lfingers[] = {HandData::LEFT_INDEX, HandData::LEFT_RING, HandData::LEFT_MIDDLE, HandData::LEFT_PINKY, HandData::LEFT_THUMB};
	static const int rfingers[] = {HandData::RIGHT_INDEX, HandData::RIGHT_RING, HandData::RIGHT_MIDDLE, HandData::RIGHT_PINKY, HandData::RIGHT_THUMB};
	float coords[5][3];

	float tmp[3];
	for (int i = 0; i < 3; ++i) tmp[i] = 0;

	float handcenter[3];
	if (hand == HandData::LEFT_HAND) h.getData(HandData::LEFT_HAND, handcenter);
	if (hand == HandData::RIGHT_HAND) h.getData(HandData::RIGHT_HAND, handcenter);

	for (int i = 0; i < 5; ++i) {
		bool unique = false;
		if (hand == HandData::LEFT_HAND && h.getData(lfingers[i], coords[i])) {
			unique = true;
		}
		else if (hand == HandData::RIGHT_HAND && h.getData(rfingers[i], coords[i])) {
			unique = true;
		}
		if (!unique) continue;

		// If unique and far enough from hand center
		for (int j = 0; j < i; ++j) {
			if (coords[j][0] == coords[i][0] && coords[j][1] == coords[i][1] && coords[j][2] == coords[i][2]) {
				unique = false;
				break;
			}
		}
		if (unique) {
			float fl = dist2(coords[i], handcenter);
			if (fl > 0.18*0.18 || fl < 0.04*0.04) continue;
			tmp[0] += coords[i][0] - handcenter[0];
			tmp[1] += coords[i][1] - handcenter[1];
			tmp[2] += coords[i][2] - handcenter[2];
			++nf;
		}
	}
	if (nf > 1) {
		for (int i = 0; i < 3; ++i) dst[i] = dst[i]/2 + tmp[i]/(2*nf);
	}
}

void GestureCtlHandler::updateHand(int n, const HandData& h) {
	int prospective = state[n];
	int nf = countFingers(h,n);
	switch (state[n]) {
		case NONE:
			if (h.getData(n, hand[n])) prospective = NEUTRAL;
			break;
		case NEUTRAL:
			if (!h.getData(n, hand[n])) prospective = NONE;
			else {
				if (countFingers(h, n) == 2) prospective = TRACKING;
				int nf = countFingers(h,n);
			}
			break;
		case TRACKING:
			if (!h.getData(n, hand[n])) prospective = NONE;
			else {
				if (nf <= 1) prospective = ENGAGED;
				else if (nf != 2) prospective = NEUTRAL;
			}
			break;
		case ENGAGED:
			if (!h.getData(n, hand[n])) prospective = NONE;
			else {
				if (nf == 2) prospective = TRACKING;
				else if (nf > 2) prospective = NEUTRAL; 
			}
			break;
	}
	int oldstate = state[n];
	if (hysteresis(n, prospective)) {
		if (state[n] == ENGAGED) {
			if (dist2(mesh->getTranslation(), cursor->getTranslation()) < THRESHOLD) {
				selected = true;
			}
		} else {
			if (oldstate == ENGAGED || oldstate == TRACKING) {
				selected = false;
			}
		}
	}
	if (state[n] == ENGAGED || state[n] == TRACKING) {
		float newloc[3];
		float tmp[3];
		float tip[3];
		getFingertips(h, n, fingertips);

		for (int i = 0; i < 3; ++i) {
			tip[i] = fingertips[i] + hand[n][i];
			tip[i] *= 1000;
		}
		tip[0] *= -1;
		tip[1] *= -1;
		GestureCoordsToWorldCoords(tip, tmp);
		WorldCoordsToScreenCoords(tmp, newloc);
		for (int i = 0; i < 3; ++i) {
			delta[i] = newloc[i] - cursor->getTranslation()[i];
		}
		/*char deb[200];
		sprintf_s(deb, "%.4f %.4f %.4f | %.4f %.4f %.4f\n", tmp[0], tmp[1], tmp[2], newloc[0], newloc[1], newloc[2]);
		OutputDebugString(deb);*/
		cursor->setTranslation(newloc);
	} else {
		selected = false;
		cursor->setTranslation(offscreen);
	}
}

inline void rotate(const float* v, const float* ax, float theta, float* dst) {
	float ctheta = cos(theta);
	float stheta = sin(theta);
	float dot = v[0]*ax[0] + v[1]*ax[1] + v[2]*ax[2];
	cross(v, ax, dst);
	dst[0] = ctheta*v[0] + ax[0]*dot*(1-ctheta) - dst[0]*stheta; 
	dst[1] = ctheta*v[1] + ax[1]*dot*(1-ctheta) - dst[1]*stheta; 
	dst[2] = ctheta*v[2] + ax[2]*dot*(1-ctheta) - dst[2]*stheta; 
}

void GestureCtlHandler::GestureCoordsToWorldCoords(const float* src, float* dst) {
// Camera to world matrix obtained from separate calibration
	dst[0] = (float)( 0.99948*src[0] - 0.01059*src[1] - 0.03078*src[2] + 29.06272);
	dst[1] = (float)(-0.02306*src[0] - 0.89894*src[1] - 0.43887*src[2] + 523.75518);
	dst[2] = (float)( 0.02326*src[0] - 0.43882*src[1] + 0.89847*src[2] - 120.88339);

	/*dst[0] = (float) (src[0] + 1.7);
	dst[1] = (float)(-0.89918*src[1] - 0.43958*src[2] + 526.52482);
	dst[2] = (float)(-0.43858*src[1] + 0.89918*src[2] - 123.67720);*/
	for (int i = 0; i < 3; ++i) {
		dst[i] = correction[i] + linear[i]*dst[i];
		dst[i] /= 1000.f;
	}
}
void GestureCtlHandler::WorldCoordsToScreenCoords(const float* src, float* dst) {	
	// Then convert screen space to display space
	// Z scale using linear regression
	dst[2] = 3.6755f*src[2] - 2.3312f;
	// dst[2] = -8.3581*dst[2]*dst[2] + 11.458*dst[2] - 4;
	// dst[2] = -3.84*dst[2]*dst[2] + 7.1921*dst[2] - 3.074;

	// Scale x, y according to z and approximate data. 
	float xscale = -0.8203f*dst[2] + 2.5743f;
	dst[0] = src[0]*xscale;
	float yscale = -0.3871f*dst[2] + 2.7963f;
	dst[1] = yscale*src[1] - 1;
}

void GestureCtlHandler::update() {
	manager->update();

	updateCursorPosition();

	if (selected) {
		mesh->setColor(0.f,1.f,0.f);
		mesh->translateBy(delta);
	} else if (dist2(mesh->getTranslation(), cursor->getTranslation()) < THRESHOLD) {
		mesh->setColor(0.f,0.f,1.f);
	} else {
		mesh->setColor(1.f,0.f,0.f);
	}

	float color[3] = {1.,1.,1.};
	if (state[0] == NEUTRAL) color[0] = 0;
	else if (state[0] == TRACKING) color[1] = 0;
	else if (state[0] == ENGAGED) {
		color[1] = 0; 
		color[2] = 0;
	}
	cursor->setColor(color[0],color[1],color[2]);
}

void GestureCtlHandler::updateCursorPosition() {
	HandData h = manager->getSkelData();
	h.getData(HandData::LEFT_TIP, hand[0]);
	h.getData(HandData::RIGHT_TIP, hand[1]);
	updateHand(0, h);
	//updateHand(1, h);
}

void GestureCtlHandler::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
	char s[120];	
	switch(sym) {
			case SDLK_RIGHT: correction[0] += 5;
						sprintf_s(s, "%.4f %.4f %.4f\n", correction[0], correction[1], correction[2]);
						//OutputDebugString(s);
						break;
			case SDLK_LEFT: correction[0] -= 5.f;
						sprintf_s(s, "%.4f %.4f %.4f\n", correction[0], correction[1], correction[2]);
						//OutputDebugString(s);
						break;
			case SDLK_UP: correction[1] += 5.f;
						sprintf_s(s, "%.4f %.4f %.4f\n", correction[0], correction[1], correction[2]);
						//OutputDebugString(s);
						break;
			case SDLK_DOWN: correction[1] -= 5.f;
				sprintf_s(s, "%.4f %.4f %.4f\n", correction[0], correction[1], correction[2]);
				//OutputDebugString(s);
				break;
			case SDLK_SLASH: correction[2] += 5.f;
				sprintf_s(s, "%.4f %.4f %.4f\n", correction[0], correction[1], correction[2]);
				//OutputDebugString(s);
				break;
			case SDLK_QUOTE: correction[2] -= 5.f;
				sprintf_s(s, "%.4f %.4f %.4f\n", correction[0], correction[1], correction[2]);
				//OutputDebugString(s);
				break;
			case SDLK_j: linear[0] += 0.05f;
				sprintf_s(s, "%.4f %.4f %.4f\n", linear[0], linear[1], linear[2]);
				//OutputDebugString(s);
				break;
			case SDLK_n: linear[0] -= 0.05f;
				sprintf_s(s, "%.4f %.4f %.4f\n", linear[0], linear[1], linear[2]);
				//OutputDebugString(s);
				break;
			case SDLK_o: linear[1] += 0.05f;
				sprintf_s(s, "%.4f %.4f %.4f\n", linear[0], linear[1], linear[2]);
				//OutputDebugString(s);
				break;
			case SDLK_k: linear[1] -= 0.05f;
				sprintf_s(s, "%.4f %.4f %.4f\n", linear[0], linear[1], linear[2]);
				//OutputDebugString(s);
				break;
			case SDLK_p: linear[2] += 0.05f;
				sprintf_s(s, "%.4f %.4f %.4f\n", linear[0], linear[1], linear[2]);
				//OutputDebugString(s);
				break;
			case SDLK_l: linear[2] -= 0.05f;
				sprintf_s(s, "%.4f %.4f %.4f\n", linear[0], linear[1], linear[2]);
				//OutputDebugString(s);
				break;
			case SDLK_ESCAPE:
			case SDLK_q:
				app->stop();
			default:
				break;
		}
}