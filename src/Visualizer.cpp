#include "Visualizer.h"

Visualizer::Visualizer(void)
{
}


Visualizer::~Visualizer(void)
{
}

void Visualizer::init(InputManager* m, Renderer* r) {
	manager = m;
	render = r;
}

void Visualizer::draw() {
	int len = manager->width()*manager->height();
	render->drawPoints(len, manager->getCoords(), manager->getRGBFrame(), 2.f);

	/*HandData hd = manager->getSkelData();
	float pts[21];
	float colors[21];
	int n = 0;

	float coords[3];
	static int pois[] = {HandData::LEFT_HAND, HandData::LEFT_TIP, HandData::LEFT_THUMB, HandData::LEFT_INDEX, HandData::LEFT_RING, HandData::LEFT_MIDDLE, HandData::LEFT_PINKY};
	static float poicols[] = {
		0,0,1,
		0,1,0,
		1,0,0,
		1,.5,.5,
		1,.5,.5,
		1,.5,.5,
		1,.5,.5,
	};
	int npoi = 7;
	for (int i = 1; i < npoi; ++i) {
		if (hd.getData(pois[i], coords)) {
			colors[n] = poicols[3*i]; colors[n+1] = poicols[3*i+1]; colors[n+2] = poicols[3*i+2];
			pts[n++] = coords[0]; pts[n++] = coords[1]; pts[n++] = coords[2];
		}
	}
	render->drawPoints(n/3, pts, colors, 10.f);*/
}