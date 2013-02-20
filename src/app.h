#pragma once
#include <SDL.h>
#include "SDLHandler.h"
#include <Windows.h>
#include <vector>

class SDLHandler;

class App {
	public: 
		App();
		App(int w, int h, SDLHandler* myhandler);
		int onExecute();
		void stop();

		virtual bool onInit();
		virtual void onLoop();
		virtual void onRender();
		virtual void onCleanup();

		int getWidth() const { return width; }
		int getHeight() const { return height; }
		bool isFullscreen() const { return fullscreen; }
	protected:
		virtual bool initSDL();
		bool running;
		bool fullscreen;
		SDLHandler* handler;
		std::vector<SDLHandler*> handlers;
		SDL_Surface* screen;

		int width;
		int height;
		Uint32 flags;
};