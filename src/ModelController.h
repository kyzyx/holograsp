#pragma once
#include "Renderer.h"
#include <vector>
#include "Mesh.h"

class ModelController
{
public:
	ModelController(void);
	~ModelController(void);

	virtual bool init(Renderer* r) { render = r; return true;}
	virtual void addMesh(Mesh* m) { meshes.push_back(m); }
	virtual void draw() { 
		for (unsigned int i = 0; i < meshes.size(); ++i) {
			meshes[i]->draw();
			meshes[i]->setColor(0.f,0.f,0.f);
			meshes[i]->translateBy(0.f,0.f,0.0001f);
			meshes[i]->drawWireframe();
			meshes[i]->translateBy(0.f,0.f,-0.0001f);
		}
	}
	virtual Mesh* getMesh(int i) { return meshes[i]; }
	virtual void debug() { ; }

protected:
	Renderer* render;
	std::vector<Mesh*> meshes;
};

