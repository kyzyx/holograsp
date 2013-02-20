#include "ExperimentalApp.h"
#include "D3DRenderer.h"
#include "D3DMesh.h"
#include "MeshCtlHandler.h"
	
static const double headdistance = 0.8;
static const double screenheight2 = 0.167;
static const double screenwidth2 = 0.3;

bool ExperimentalApp::onInit() {
	if (!initSDL()) return false;
	s = NULL;
	try {
		drender = new D3DRenderer(this);
		orthoview = drender->InitializeWindowTarget(320, 240);
		render = drender;
	} catch (DirectXInitException e) {
		return false;
	}
	shandler = new StatusScreenHandler();
	handler = shandler;

	fov = 2*atan(screenheight2/headdistance);
	render->setProjection(fov, 16.f/9.f);

	eye[0] = 0.f; eye[1] = 0.f; eye[2] = (float) headdistance;
	towards[0] = 0.f; towards[1] = 0.f; towards[2] = -1;
	up[0] = 0.f; up[1] = 1.f; up[2] = 0.f;

	sideeye[0] = screenwidth2; sideeye[1] = 0.f; sideeye[2] = (float)(-2*headdistance);
	sidetowards[0] = -1.f; sidetowards[1] = 0.f; sidetowards[2] = 0.f;
	sideup[0] = up[0]; sideup[1] = up[1]; sideup[2] = up[2];

	topeye[0] = 0.f; topeye[1] = 0; topeye[2] = (float)(-headdistance);
	toptowards[0] = 0.f; toptowards[1] = -1.f; toptowards[2] = 0.f;
	topup[0] = 0.f; topup[1] = 0.f; topup[2] = -1.f;

	setMainCamera();

	if (!use3d) {
		drender->disableStereo();
	} else {
		drender->enableStereo();
	}
	return true;
}

void ExperimentalApp::setMainCamera() {
	render->setProjection(fov, 16.f/9.f);
	render->lookAt(eye, towards, up);
}
void ExperimentalApp::setTopCamera() {
	render->ortho(6*screenwidth2, 2*headdistance);
	render->lookAt(topeye, toptowards, topup);
}
void ExperimentalApp::setSideCamera() {
	render->ortho(headdistance*2, 6*screenheight2);
	render->lookAt(sideeye, sidetowards, sideup);
}


void ExperimentalApp::onLoop() {
	if (stage == STAGE_TEST) {
		s->setState(getState());
		s->updateTask();

		if (s->isDone()) {
			stage = STAGE_POSTTEST;
			handler = shandler;
			delete s;
			s = NULL;
		}
	} else if (stage == STAGE_POSTTEST) {
		if (shandler->status() == 2) {
			stop();
		}
	} else if (stage == STAGE_PRETEST) {
		if (shandler->status() == 1) {
			try {
				s = new SpeedTestController(render);
			} catch (OFFFileFormatError e) {
				//OutputDebugString(e.err.c_str());
				stage = STAGE_ERROR;
				return;
			}		
			if (!use3d) {
				drender->disableStereo();
			} else {
				drender->enableStereo();
			}
			char fname[100];
			sprintf_s(fname, "%s_%dd%s_%d.txt", netid, use3d?3:2, getName(), round);
			if (!s->init(fname, 11)) {
				stage = STAGE_ERROR;
				return;
			}
			if (!initControl()) { // SWITCHES HANDLER!
				stage = STAGE_ERROR;
			} else {
				handler = myhandler;
				stage = STAGE_TEST;
				render->setClearColor(0.15f,0.15f,0.15f);
			}
		}
	}
}

void ExperimentalApp::onRender(){
	render->clear();
	if (stage == STAGE_TEST) {
		drender->setRenderTarget(orthoview->rt);
		drender->setClearColor(0.f,0.f,0.f);
		drender->clearRenderTarget(orthoview->rt);
		// Set new camera view
		setTopCamera();
		//setSideCamera();
		s->draw();

		drender->setRenderTarget();
		drender->setClearColor(0.15f,0.15f,0.15f);
		render->clear();
		setMainCamera();
		s->draw();
		if (!use3d) {
			drender->DrawWindowTarget(orthoview, 0,0);
		}
	} else if (stage == STAGE_POSTTEST) {
		render->drawText(L"Thanks for taking the test! You can remove the glasses now", width/2.f, height/3.f, 0xff0000ff, 40.f);
	} else if (stage == STAGE_PRETEST) {
		render->drawText(L"Welcome to the Test\nPlease put on the 3D glasses.", width/2.f, height/3.f, 0xff0000ff, 40.f);
		render->drawText(L"Press SPACE to continue!", width/2.f, 3*height/4.f, 0xff0000ff, 32.f);
	} else if (stage == STAGE_ERROR) {
		render->drawText(L"Oops! Something went wrong. Please check with Edward.", width/2.f, height/2.f, 0xff0000ff, 32.f);
	}
	render->display();
}