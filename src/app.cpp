#include <SDL.h>
#include "app.h"

App::App() {
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	fullscreen = false;
	flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	//handler = new SDLHandler(this);
}

App::App(int w, int h, SDLHandler* myhandler) {
	running = true;
	screen = NULL;
	width = w;
	height = h;
	flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	handler = myhandler;
}

void App::stop() {
	running = false;
}

int App::onExecute() {
	if (!onInit()) stop();
	SDL_Event ev;
	while (running) {
		while (SDL_PollEvent(&ev)) {
			if (handler) handler->OnEvent(&ev);
		}
		if (handler) handler->update();
		onLoop();
		onRender();
	}
	onCleanup();
	return 0;
}

bool App::onInit() {
	return initSDL();
}

bool App::initSDL(){
	if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	if (!info) return false;
	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
		width = info->current_w;
		height = info->current_h;
	}
	screen = SDL_SetVideoMode(width, height, info->vfmt->BitsPerPixel, flags);
	if (!screen) return false;
	else return true;
}

void App::onLoop(){
}
void App::onRender(){
}
void App::onCleanup(){
	SDL_Quit();
	if (handler) {
		delete handler;
		handler = NULL;
	}
}