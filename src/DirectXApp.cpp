#include "DirectXApp.h"
#include "D3DRenderer.h"
#include "CameraCtlHandler.h"



DirectXApp::DirectXApp(void) {
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	flags = 0;
	fullscreen = false;

	render = NULL;
}

DirectXApp::DirectXApp(bool fullScreen) {
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	fullscreen = fullScreen;
	flags = 0;

	render = NULL;
}

DirectXApp::~DirectXApp(void)
{
	if (handler) {
		delete handler;
		handler = NULL;
	}
	if (render) {
		delete render;
		render = NULL;
	}
}

bool DirectXApp::onInit(void) {
	if (!initSDL()) return false;
	
	try {
		render = new D3DRenderer(this);
	} catch (DirectXInitException e) {
		return false;
	}

	handler = new CameraCtlHandler(this, (Renderer*) render, width, height, 45);
	return true;
}

void DirectXApp::onRender() {
	render->clear();

	render->display();
}