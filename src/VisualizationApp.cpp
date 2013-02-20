#include "VisualizationApp.h"
#include "D3DRenderer.h"
#include "CameraCtlHandler.h"
#include "ClosestPointCtlHandler.h"
inline double randDouble() {
	int r1 = rand();
    double r2 = ((double) rand()) / ((double) RAND_MAX);
    return (r1 + r2) / ((double) RAND_MAX);
}
class SpeedTestControllerVis : public SpeedTestController {
public:
	SpeedTestControllerVis(Renderer* render) : SpeedTestController(render) {
		;
	}
protected:
	void startTask() {
		float x, y, z;
		do {
			x = 0.5*randDouble()-0.25; y = 0.5*randDouble()-0.25; z = 0.4*randDouble()+0.2;
		} while (!inView(x, y, z, TARGETSCALE));
		target->setTranslation(x,y,z);
		do {
			x = 0.5*randDouble()-0.25; y = 0.5*randDouble()-0.25; z = 0.4*randDouble();
		} while (!inView(x, y, z, DESTSCALE));
		dest->setTranslation(x,y,z);
		out << round << ": ";
		starttime = clock();
		state = STATE_NONE;
	}

	virtual bool inView(double x, double y, double z, double r) {
		static double tt = tan(3.1415926536/8);
		return x > -z*tt && x < z*tt && y > -z*tt && y < z*tt;
	}
};

VisualizationApp::VisualizationApp(void)
{
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	flags = SDL_OPENGL;
	fullscreen = false;
}


VisualizationApp::~VisualizationApp(void)
{
}

bool VisualizationApp::onInit() {
	if (!initSDL()) return false;
	sc = NULL;
	//render = new GLRenderer(this);
	try {
		render = new D3DRenderer(this);
	} catch (DirectXInitException e) {
		return false;
	}
	manager = new UtilPipelineInputManager();
	if (!manager->init()) {
		//OutputDebugString("Failed to initialize depth camera\n");
		throw std::exception();
	}
	sc = new SpeedTestControllerVis(render);
	sc->init("test", 11);
	vis = new Visualizer();
	vis->init(manager, render);
	handler = new CameraCtlHandler(this, render, width, height, 45);
	handlers.push_back(new ClosestPointCtlHandler(this, sc->getMesh(SpeedTestController::MESH_TARGET), sc->getMesh(SpeedTestController::MESH_CURSOR), manager));
	return true;
}
void VisualizationApp::onLoop() {
	manager->update();
	sc->updateTask();
}
void VisualizationApp::onRender() {
	render->clear();
	vis->draw();
	sc->draw();
	render->display();
}
