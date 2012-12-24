#include "HandController.h"
#include <cstdlib>
#include <Windows.h>
#include <cstdio>

#include "D3DMesh.h"

const int CLOSEST_WINDOW = 1;
const float NEAR_THRESH = 0.001f;
const float FAR_THRESH = 0.6f;
HandController::HandController(void)
{
	state[0] = NONE;
	state[1] = NONE;
	newstate[0] = NONE;
	newstate[1] = NONE;
	counter[0] = 0;
	counter[1] = 0;
	threshold = 5;
}


HandController::~HandController(void)
{
}

inline float dist2(float a[3], float b[3]) {
	return (a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]);
}
// Two fingers, close together
// Two fingers, noisy
// More fingers, close to center?
// Finger confidences?
int countFingers(const HandData& h, int hand) {
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

void HandController::updateHand(int n, const HandData& h) {
	int prospective = state[n];
	float coords[3];
	switch (state[n]) {
		case NONE:
			if (h.getData(n, coords)) prospective = NEUTRAL;
			break;
		case NEUTRAL:
			if (!h.getData(n, coords)) prospective = NONE;
			else {
				if (countFingers(h, n) == 2) prospective = TRACKING;
				int nf = countFingers(h,n);
			}
			break;
		case TRACKING:
			if (!h.getData(n, coords)) prospective = NONE;
			else {
				int nf = countFingers(h,n);
				if (nf <= 1) prospective = ENGAGED;
				else if (nf != 2) prospective = NEUTRAL;
			}
			break;
		case ENGAGED:
			if (!h.getData(n, coords)) prospective = NONE;
			else {
				int nf = countFingers(h,n);
				if (nf == 2) prospective = TRACKING;
				else if (nf > 2) prospective = NEUTRAL; 
			}
			break;
	}
	int oldstate = state[n];
	if (hysteresis(n, prospective)) {
		if (state[n] == ENGAGED) {
			// Record starting position
		}
	}
}

void HandController::update() {
	/*HandData h = manager->getSkelData();
	h.getData(HandData::LEFT_TIP, hand[0]);
	h.getData(HandData::RIGHT_TIP, hand[1]);
	updateHand(0, h);
	updateHand(1, h);*/
	state[0] = TRACKING;
	closestPoint();

	if (state[0] == ENGAGED) {
		meshes[0]->setTranslation(hand[0]);
	}
}

bool HandController::init(InputManager* m, Renderer* r) {
	manager = m;
	render = r;

	return true;
}

void HandController::debug() {
	float color[3] = {0.,0.,0.};
	if (state[0] == NEUTRAL) color[0] = 1;
	else if (state[0] == TRACKING) color[1] = 1;
	else if (state[0] == ENGAGED) color[2] = 1;
	if (state[0] != NONE) render->drawPoints(1, hand[0], color, 3.f);
}

void HandController::closestPoint() {
	static const float origin[] = {0.f,0.f,0.f};
	const float* verts = manager->getCoords();
	int w = manager->width();
	int h = manager->height();

	int mini = -1; int minj = -1;
	float mindist = 999.f;
	float bestcoords[3];

	float coords[3];
	float mindcoords[3]; float maxdcoords[3];

	for (int i = CLOSEST_WINDOW; i < h - CLOSEST_WINDOW; ++i) {
		for (int j = CLOSEST_WINDOW; j < w - CLOSEST_WINDOW; ++j) {
			float dmax = 0.0f; float dmin = 999.f;
			float dtot = 0.0f;
			int n = 0;
			for (int x = -CLOSEST_WINDOW; x <= CLOSEST_WINDOW; ++x) {
				for (int y = -CLOSEST_WINDOW; y <= CLOSEST_WINDOW; ++y) {
					int k = (j+y) + w*(i+x);
					float d = dist2(origin, verts+k*3);
					if (d < NEAR_THRESH || d > FAR_THRESH || (i > 205 && d > 0.35)) continue;
					++n;
					dtot += d;
					if (d > dmax) {
						dmax = d;
					}
					if (d < dmin) {
						dmin = d;
					}
				}
			}
			dtot -= dmax + dmin;
			n -= 2;
			if (n > 0) {
				dtot /= n;
				if (dtot < mindist) {
					mindist = dtot;
					mini = i;
					minj = j;
				}
			}
		}
	}

	if (mini > -1) {
		for (int a = 0; a < 3; ++a) {
			coords[a] = 0;
			mindcoords[a] = 0;
			maxdcoords[a] = 0;
		}
		int n = 0;
		float dmax = 0.f; float dmin = 999.f;
		for (int x = -CLOSEST_WINDOW; x <= CLOSEST_WINDOW; ++x) {
			for (int y = -CLOSEST_WINDOW; y <= CLOSEST_WINDOW; ++y) {
				int k = (minj+y) + w*(mini+x);
				float d = dist2(origin, verts+k*3);
				if (d < NEAR_THRESH || d > FAR_THRESH || (mini > 205 && d > 0.35)) continue;
				++n;
				for (int a = 0; a < 3; ++a) coords[a] += *(verts+3*k+a);
				if (d > dmax) {
					for (int a = 0; a < 3; ++a) maxdcoords[a] = *(verts+3*k+a);
					dmax = d;
				}
				if (d < dmin) {
					for (int a = 0; a < 3; ++a) mindcoords[a] = *(verts+3*k+a);
					dmin = d;
				}
			}
		}
		n -= 2;
		for (int a = 0; a < 3; ++a) {
			bestcoords[a] = (coords[a] - mindcoords[a] - maxdcoords[a])/n;
		}

		for (int i = 0; i < 3; ++i) hand[0][i] = bestcoords[i];
		char tmps[120];
		sprintf_s(tmps, "%d %d: %.4f %.4f %.4f: %.4f\n", mini, minj, hand[0][0], hand[0][1], hand[0][2], dist2(origin, hand[0]));
		OutputDebugString(tmps);
	}
}