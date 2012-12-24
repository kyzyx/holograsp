#pragma once
#include "ModelController.h"
#include <iostream>
#include <fstream>
#include <ctime>

class SpeedTestController : public ModelController
{
public:
	SpeedTestController(Renderer* render);
	~SpeedTestController(void);

	virtual bool init(const char* filename, int numrounds);
	virtual void updateTask();
	virtual bool isDone() { return round >= numrounds;}
	virtual void tick();
	virtual void draw();
	enum {
		MESH_DEST,
		MESH_TARGET,
		MESH_CURSOR,
	};

	enum {
		STATE_NONE,
		STATE_TRACKING,
		STATE_ENGAGED,
	};
	void setState(int n);
private:
	bool isDoneTask(); // Returns if target is inside dest satisfactorily
	void startTask();  // Begins a task: Sets timers and randomizes positions
	bool inView(double x, double y, double z, double r);     // Returns whether a placement of a sphere is in the valid 3d view area

	static const float DESTSCALE;
	static const float TARGETSCALE;
	static const float CURSORSCALE;
	static const double THRESHOLD;

	Mesh* target;
	Mesh* dest;
	Mesh* cursor;

	int round;
	int numrounds;
	clock_t starttime;
	int state;

	std::ofstream out;
};

