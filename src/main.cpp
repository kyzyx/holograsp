#include "DirectXApp.h"
#include "ExperimentalApp.h"
#include "VisualizationApp.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	App* app;
	try{
		//app = new GestureExpApp(true, "test", 2);
        app = new CPExpApp(true, "test", 2);
		//app = new VisualizationApp();
	} catch (std::exception e) {
		return 0;
	}
	int ret = app->onExecute();
	delete app;
	return ret;
}
