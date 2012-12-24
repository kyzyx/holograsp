#include "StereoApp.h"
#include "UtilPipelineInputManager.h"
#include "CameraCtlHandler.h"
#include "HandController.h"
#include "D3DRenderer.h"
#include "D3DMesh.h"

StereoApp::StereoApp(void) : DirectXApp() {
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	fullscreen = true;
	flags = 0;
	
	manager = NULL;
	handler = NULL;
	ctl = NULL;
	mesh = NULL;
}

StereoApp::~StereoApp(void)
{
	if (handler) { delete handler; handler = NULL; }
	if (ctl) delete ctl;
	if (manager) delete manager;
	if (mesh) delete mesh;
}

bool StereoApp::onInit() {
	if (!initSDL()) return false;
	try {
		render = new D3DRenderer(this);
	} catch (DirectXInitException e) {
		return false;
	}
	try {
		mesh = new D3DMesh("tetra.off", (D3DRenderer*) render);
	} catch (OFFFileFormatError e) {
		OutputDebugString(e.err.c_str());
		throw e;
	}
	manager = new UtilPipelineInputManager();
	if (!manager->init()) {
		OutputDebugString("Failed to initialize depth camera\n");
		return false;
	}
	
	// Other object initialization
	
	handler = new CameraCtlHandler(this, (Renderer*) render, width, height, 45);
	vis.init(manager, (Renderer*) render);
	ctl = new HandController();
	ctl->init(manager, (Renderer*) render);
	mesh->setScale(0.05f);
	ctl->addMesh(mesh);
	
	return true;
}

//--------------------------------------------------------------
void StereoApp::onLoop(){
	manager->update();
	ctl->update();
}

//--------------------------------------------------------------
void StereoApp::onRender(){
	render->clear();
	vis.draw();
	ctl->debug();
	ctl->draw();
	render->display();
}

