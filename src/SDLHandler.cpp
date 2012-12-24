#include "SDLHandler.h"


SDLHandler::SDLHandler(void)
{
}

SDLHandler::SDLHandler(App* myapp)
{
	app = myapp;
}

SDLHandler::~SDLHandler(void)
{
}

void SDLHandler::update() {
};

void SDLHandler::OnEvent(SDL_Event* Event) {
    switch(Event->type) {
        case SDL_ACTIVEEVENT: {
            switch(Event->active.state) {
                case SDL_APPMOUSEFOCUS: {
                    if ( Event->active.gain )    OnMouseFocus();
                    else                OnMouseBlur();
 
                    break;
                }
                case SDL_APPINPUTFOCUS: {
                    if ( Event->active.gain )    OnInputFocus();
                    else                OnInputBlur();
 
                    break;
                }
                case SDL_APPACTIVE:    {
                    if ( Event->active.gain )    OnRestore();
                    else                OnMinimize();
 
                    break;
                }
            }
            break;
        }
 
        case SDL_KEYDOWN: {
            OnKeyDown(Event->key.keysym.sym,Event->key.keysym.mod,Event->key.keysym.unicode);
            break;
        }
 
        case SDL_KEYUP: {
            OnKeyUp(Event->key.keysym.sym,Event->key.keysym.mod,Event->key.keysym.unicode);
            break;
        }
 
        case SDL_MOUSEMOTION: {
            OnMouseMove(Event->motion.x,Event->motion.y,Event->motion.xrel,Event->motion.yrel,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_LEFT))!=0,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_RIGHT))!=0,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_MIDDLE))!=0);
            break;
        }
 
        case SDL_MOUSEBUTTONDOWN: {
            switch(Event->button.button) {
                case SDL_BUTTON_LEFT: {
                    OnLButtonDown(Event->button.x,Event->button.y);
                    break;
                }
                case SDL_BUTTON_RIGHT: {
                    OnRButtonDown(Event->button.x,Event->button.y);
                    break;
                }
                case SDL_BUTTON_MIDDLE: {
                    OnMButtonDown(Event->button.x,Event->button.y);
                    break;
                }
				case SDL_BUTTON_WHEELUP: {
					OnMouseWheel(true, false);
					break;
				}
				case SDL_BUTTON_WHEELDOWN: {
					OnMouseWheel(false, true);
					break;
				}
            }
            break;
        }
 
        case SDL_MOUSEBUTTONUP:    {
            switch(Event->button.button) {
                case SDL_BUTTON_LEFT: {
                    OnLButtonUp(Event->button.x,Event->button.y);
                    break;
                }
                case SDL_BUTTON_RIGHT: {
                    OnRButtonUp(Event->button.x,Event->button.y);
                    break;
                }
                case SDL_BUTTON_MIDDLE: {
                    OnMButtonUp(Event->button.x,Event->button.y);
                    break;
                }
            }
            break;
        }
 
        case SDL_JOYAXISMOTION: {
            OnJoyAxis(Event->jaxis.which,Event->jaxis.axis,Event->jaxis.value);
            break;
        }
 
        case SDL_JOYBALLMOTION: {
            OnJoyBall(Event->jball.which,Event->jball.ball,Event->jball.xrel,Event->jball.yrel);
            break;
        }
 
        case SDL_JOYHATMOTION: {
            OnJoyHat(Event->jhat.which,Event->jhat.hat,Event->jhat.value);
            break;
        }
        case SDL_JOYBUTTONDOWN: {
            OnJoyButtonDown(Event->jbutton.which,Event->jbutton.button);
            break;
        }
 
        case SDL_JOYBUTTONUP: {
            OnJoyButtonUp(Event->jbutton.which,Event->jbutton.button);
            break;
        }
 
        case SDL_QUIT: {
            OnExit();
            break;
        }
 
        case SDL_SYSWMEVENT: {
            //Ignore
            break;
        }
 
        case SDL_VIDEORESIZE: {
            OnResize(Event->resize.w,Event->resize.h);
            break;
        }
 
        case SDL_VIDEOEXPOSE: {
            OnExpose();
            break;
        }
 
        default: {
            OnUser(Event->user.type,Event->user.code,Event->user.data1,Event->user.data2);
            break;
        }
    }
}
 
void SDLHandler::OnInputFocus() {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnInputBlur() {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnMouseFocus() {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnMouseBlur() {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnMouseMove(int mX, int mY, int relX, int relY, bool Left,bool Right,bool Middle) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnMouseWheel(bool Up, bool Down) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnLButtonDown(int mX, int mY) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnLButtonUp(int mX, int mY) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnRButtonDown(int mX, int mY) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnRButtonUp(int mX, int mY) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnMButtonDown(int mX, int mY) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnMButtonUp(int mX, int mY) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnJoyAxis(Uint8 which,Uint8 axis,Sint16 value) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnJoyButtonDown(Uint8 which,Uint8 button) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnJoyButtonUp(Uint8 which,Uint8 button) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnJoyHat(Uint8 which,Uint8 hat,Uint8 value) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnJoyBall(Uint8 which,Uint8 ball,Sint16 xrel,Sint16 yrel) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnMinimize() {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnRestore() {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnResize(int w,int h) {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnExpose() {
    //Pure virtual, do nothing
}
 
void SDLHandler::OnExit() {
    app->stop();
}
 
void SDLHandler::OnUser(Uint8 type, int code, void* data1, void* data2) {
    //Pure virtual, do nothing
}