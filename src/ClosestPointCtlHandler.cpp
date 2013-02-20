#include "ClosestPointCtlHandler.h"

const int CLOSEST_WINDOW = 1;
const float NEAR_THRESH = 0.001f;
const float FAR_THRESH = 0.6f;

ClosestPointCtlHandler::ClosestPointCtlHandler(App* myapp, Mesh* m, Mesh* c, InputManager* mgr) 
	: GestureCtlHandler(myapp, m, c, mgr)
{
	for (int i = 0; i < 3; ++i) {
		coordfilters[i] = new DoubleAvgFilter(8);
		//coordfilters[i] = new AdaptiveDoubleExpFilter(0.3, 0.9, 0.5, 0.9, 0.02, 0.4);
	}
	visualize = false;
}
ClosestPointCtlHandler::~ClosestPointCtlHandler(void) {
	for (int i = 0; i < 3; ++i) delete coordfilters[i];
}

void ClosestPointCtlHandler::updateCursorPosition() {
	// Find Closest Window
	static const float origin[] = {0.f,0.f,0.f};
	const float* verts = manager->getCoords();
	int w = manager->width();
	int h = manager->height();

	int mini = -1; int minj = -1;
	float mindist = 999.f;


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


	if (mini < 0) {
		selected = false;
		for (int i = 0; i < 3; ++i) coordfilters[i]->reset();
	} else {
		float coords[3];
		float mindcoords[3]; float maxdcoords[3];
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
		for (int i = 0; i < 3; ++i) {
			coordfilters[i]->update((coords[i] - mindcoords[i] - maxdcoords[i])/n);
		}

		float tip[3];
		float newloc[3];
		float tmp[3];
		for (int i = 0; i < 3; ++i) {
			tip[i] = (float)(1000*coordfilters[i]->get());
		}
		if (!visualize) {
			tip[0] *= -1; tip[1] *= -1;
			GestureCoordsToWorldCoords(tip, tmp);
			WorldCoordsToScreenCoords(tmp, newloc);
		} else {
			for (int i = 0; i < 3; ++i) newloc[i] = tip[i]/1000.;
		}
		for (int i = 0; i < 3; ++i) delta[i] = newloc[i] - cursor->getTranslation()[i];
		cursor->setTranslation(newloc);
	}
}

void ClosestPointCtlHandler::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
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
		case SDLK_SPACE:
            if (dist2(mesh->getTranslation(), cursor->getTranslation()) < THRESHOLD) {
                selected = true;
            }
			break;
		case SDLK_RETURN:
			{
				sprintf_s(s, "%.4f %.4f %.4f\n", coordfilters[0]->get(), coordfilters[1]->get(), coordfilters[2]->get());
				//OutputDebugString(s);
				break;
			}
		case SDLK_ESCAPE:
			app->stop();
		default:
			break;
	}
}
void ClosestPointCtlHandler::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) {
		case SDLK_SPACE:
			selected = false;
			break;
		case SDLK_ESCAPE:
		case SDLK_q:
			app->stop();
		default:
			break;
	}
}
