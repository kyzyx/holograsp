#include "ModelController.h"


ModelController::ModelController(void)
{
}


ModelController::~ModelController(void)
{
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		delete meshes[i];
	}
}
