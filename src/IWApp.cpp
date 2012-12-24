#include "IWApp.h"
#include "GLRenderer.h"
#include "UtilPipelineInputManager.h"
#include "CameraCtlHandler.h"
#include "HandController.h"
#include "GLMesh.h"

IWApp::IWApp(void)
{
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	flags = SDL_OPENGL;
	ctl = new HandController();

	manager = NULL;
	handler = NULL;
}


IWApp::~IWApp(void)
{
	if (handler) { delete handler; handler = NULL; }
	if (ctl) delete ctl;
	if (manager) delete manager;
}

//--------------------------------------------------------------
bool IWApp::onInit(){
	manager = new UtilPipelineInputManager();
	if (!manager->init()) return false;
	GLApp::onInit();
	handler = new CameraCtlHandler(this, render, width, height, 45);
	vis.init(manager, render);
	ctl->init(manager, render);
	
	try {
		Mesh* mesh = new GLMesh("tetra.off", render, true);
		mesh->setScale(0.05f);
		ctl->addMesh(mesh);
	} catch (OFFFileFormatError e) {
		OutputDebugString(e.err.c_str());
		throw e;
	}
	return true;
}

//--------------------------------------------------------------
void IWApp::onLoop(){
	manager->update();
	ctl->update();
}

//--------------------------------------------------------------
void IWApp::onRender(){
	render->clear();
	vis.draw();
	ctl->debug();
	ctl->draw();
	render->display();
}