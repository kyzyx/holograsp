#pragma once
#include "directxapp.h"
#include "Mesh.h"
#include "MeshCtlHandler.h"
#include "GestureCtlHandler.h"
#include "SpeedTestController.h"
#include "ClosestPointCtlHandler.h"
#include "UtilPipelineInputManager.h"
#include "StatusScreenHandler.h"
#include <exception>

class D3DRenderer;
class D3DWindowTarget;

class ExperimentalApp :
	public DirectXApp
{
public:
	ExperimentalApp(bool is3d = false, const char* user=NULL, int rnd=1) {
		width = 1920;
		height = 1080;
		fullscreen = true;
		handler = NULL;
		s = NULL;
		stage = STAGE_PRETEST;
		shandler = NULL;
		myhandler = NULL;
		netid = user;
		use3d = is3d;
		round = rnd;
	}
	~ExperimentalApp(void) {
		if (shandler) {
			delete shandler;
		}
		if (myhandler) {
			delete myhandler;
		}
	}

	virtual bool onInit();
	virtual void onLoop();
	virtual void onRender();
	enum {
		STAGE_ERROR,
		STAGE_PRETEST,
		STAGE_TEST,
		STAGE_POSTTEST,
	};
protected:
	virtual bool initControl() = 0;
	virtual int getState() = 0;
	virtual char* getName() = 0;

	void setMainCamera();
	void setSideCamera();
	void setTopCamera();
	
	D3DRenderer* drender;
	D3DWindowTarget* orthoview;
	StatusScreenHandler* shandler;
	SDLHandler* myhandler;
	SpeedTestController* s;
	int stage;
	const char* netid;
	float eye[3];
	float up[3];
	float towards[3];

	float topeye[3];
	float toptowards[3];
	float topup[3];

	float sideeye[3];
	float sidetowards[3];
	float sideup[3];
	double fov;

	int round;
	bool use3d;
};

class GestureExpApp : public ExperimentalApp {
public:
	GestureExpApp(bool is3d) : ExperimentalApp(is3d) {
		manager = NULL;
	}
	~GestureExpApp() {
		if (manager) delete manager;
	}
protected:
	virtual bool initControl() {
		manager = new UtilPipelineInputManager();
		if (!manager->init()) {
			OutputDebugString("Failed to initialize depth camera\n");
			return false;
		}
		myhandler = new GestureCtlHandler(this, s->getMesh(SpeedTestController::MESH_TARGET), s->getMesh(SpeedTestController::MESH_CURSOR), manager);
		return true;
	}
	virtual int getState() {
		int n = ((GestureCtlHandler*) myhandler)->getState();
		if (n == GestureCtlHandler::ENGAGED) return SpeedTestController::STATE_ENGAGED;
		else if (n == GestureCtlHandler::TRACKING) return SpeedTestController::STATE_TRACKING;
		else return SpeedTestController::STATE_NONE;
	}
	virtual char* getName() {
		return "Grab";
	}
	UtilPipelineInputManager* manager;
};

class MouseExpApp : public ExperimentalApp {
public:
	MouseExpApp(bool is3d, const char* user, int n) : ExperimentalApp(is3d, user, n) {
		;
	}
protected:
	virtual char* getName() {
		return "mouse";
	}
	virtual bool initControl() {
		myhandler = new MeshCtlHandler(this,s->getMesh(SpeedTestController::MESH_TARGET), s->getMesh(SpeedTestController::MESH_CURSOR));
		return true;
	}
	virtual int getState() {
		if (((MeshCtlHandler*) myhandler)->isSelected()) return SpeedTestController::STATE_ENGAGED;
		else return SpeedTestController::STATE_TRACKING;
	}
};

class CPExpApp : public ExperimentalApp {
public:
	CPExpApp(bool is3d, const char* user, int n) : ExperimentalApp(is3d, user, n) {
		manager = new UtilPipelineInputManager();
		if (!manager->init()) {
			OutputDebugString("Failed to initialize depth camera\n");
			throw std::exception();
		}
	}
	~CPExpApp() {
		if (manager) {
			delete manager;
		}
	}
protected:
	virtual bool initControl() {
		myhandler = new ClosestPointCtlHandler(this, s->getMesh(SpeedTestController::MESH_TARGET), s->getMesh(SpeedTestController::MESH_CURSOR), manager);
		return true;
	}
	virtual int getState() {
		int n = ((ClosestPointCtlHandler*) myhandler)->getState();
		if (n == GestureCtlHandler::ENGAGED) return SpeedTestController::STATE_ENGAGED;
		else if (n == GestureCtlHandler::TRACKING) return SpeedTestController::STATE_TRACKING;
		else return SpeedTestController::STATE_NONE;
	}
	virtual char* getName() {
		return "gesture";
	}
	UtilPipelineInputManager* manager;
};