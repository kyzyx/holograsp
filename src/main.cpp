#include "IWApp.h"
#include "StereoApp.h"
#include "DirectXApp.h"
#include "ExperimentalApp.h"
const int CLOSEST_WINDOW = 1;
const float NEAR_THRESH = 0.001f;
const float FAR_THRESH = 0.6f;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	//IWApp app;
	//StereoApp app;
	//DirectXApp app(true);
	//MouseExpApp app;
	//GestureExpApp app;
	// CPExp
	App* app;
	try{
		//app = new CPExpApp();
		app = new MouseExpApp(true, "cwan", 2);
	} catch (std::exception e) {
		return 0;
	}
	int ret = app->onExecute();
	delete app;
	return ret;
}