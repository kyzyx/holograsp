#include "SpeedTestController.h"
#include "D3DMesh.h"


const float SpeedTestController::DESTSCALE = 0.04f;
const float SpeedTestController::TARGETSCALE = 0.04f;
const float SpeedTestController::CURSORSCALE = 0.01f;
const double SpeedTestController::THRESHOLD= 0.005f;

SpeedTestController::SpeedTestController(Renderer* render)
{
	srand(time(0));
	try {
		dest = new D3DMesh("sphere.off", (D3DRenderer*) render, true);
		dest->scaleBy(DESTSCALE);
		dest->setColor(0.0f,1.0f,1.0f,1.0f);
		meshes.push_back(dest);
		target = new D3DMesh("ico.off", (D3DRenderer*) render, true);
		target->scaleBy(TARGETSCALE);
		meshes.push_back(target);
		cursor = new D3DMesh("tetra.off", (D3DRenderer*) render, true);
		cursor->scaleBy(CURSORSCALE);
		meshes.push_back(cursor);
	} catch (OFFFileFormatError e) {
		OutputDebugString(e.err.c_str());
		throw e;
	}
}

SpeedTestController::~SpeedTestController(void)
{
}

bool SpeedTestController::init(const char* filename, int nr) {
	round = 0;
	numrounds = nr;
	starttime = 0;

	out = std::ofstream(filename);
	out << filename << std::endl;

	startTask();
	return true;
/* Output Format:
Filename
trialid: acquiretime [acquiretime acquiretime] endtime
trialid: acquiretime [acquiretime acquiretime] endtime
trialid: acquiretime [acquiretime acquiretime] endtime
trialid: acquiretime [acquiretime acquiretime] endtime
...
*/
}

bool SpeedTestController::isDoneTask() {
	if (state != STATE_ENGAGED && dist2(target->getTranslation(), dest->getTranslation()) < THRESHOLD) return true;
	return false;
}

inline double randDouble() {
	int r1 = rand();
    double r2 = ((double) rand()) / ((double) RAND_MAX);
    return (r1 + r2) / ((double) RAND_MAX);
}

void SpeedTestController::startTask() {
	float x, y, z;
	do {
		x = 2*randDouble()-1; y = 2*randDouble()-1; z = -2*randDouble();
	} while (!inView(x, y, z, TARGETSCALE));
	target->setTranslation(x,y,z);
	do {
		x = 2*randDouble()-1; y = 2*randDouble()-1; z = -2*randDouble();
	} while (!inView(x, y, z, DESTSCALE));
	dest->setTranslation(x,y,z);
	out << round << ": ";
	starttime = clock();
	state = STATE_NONE;
}

void SpeedTestController::updateTask() {
	if (isDoneTask()) {
		out << (clock() - starttime)*1000/CLOCKS_PER_SEC << std::endl;
		++round;
		if (isDone()) return;
		startTask();
	}
	else if (dist2(cursor->getTranslation(), dest->getTranslation()) < THRESHOLD || dist2(target->getTranslation(), dest->getTranslation()) < THRESHOLD) {
		dest->setColor(0.0f, 0.6f, 0.6f);
	} else {
		dest->setColor(0.0f, 1.0f, 1.0f);
	}
}

bool SpeedTestController::inView(double x, double y, double z, double r) {
	if (z+r > -0.02 || z - r < -1.34) return false;
	float zz = -(z + 0.02)/1.32;
	// Approx x scale
	float xlim = 0.1980*(1-zz) + 0.7060*zz;
	if (x+r > xlim || x-r < -xlim) return false;
	// Approx y scale
	float ylim = 0.14*(1-zz) + 0.41*zz;
	if (y+r > ylim || y-r < -ylim) return false;
	return true;
}

void SpeedTestController::tick() {
	out << (clock() - starttime)*1000/CLOCKS_PER_SEC << " ";
}

void SpeedTestController::setState(int newstate) {
	if (state != STATE_ENGAGED && newstate == STATE_ENGAGED) tick();
	state = newstate;
}

void SpeedTestController::draw()  { 
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		meshes[i]->draw();
		if (i != MESH_DEST) {
			float oldcol[3];
			for (int j = 0; j < 3; ++j) oldcol[j] = meshes[i]->getColor()[j];
			meshes[i]->setColor(0.f,0.f,0.f);
			meshes[i]->translateBy(0.f,0.f,0.0001f);
			meshes[i]->drawWireframe();
			meshes[i]->translateBy(0.f,0.f,-0.0001f);
			meshes[i]->setColor(oldcol[0], oldcol[1], oldcol[2]);
		}
	}
}