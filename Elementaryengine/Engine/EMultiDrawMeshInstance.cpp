#include "EMultiDrawMeshInstance.h"



EMultiDrawMeshInstance::EMultiDrawMeshInstance(Mesh * referenced)
{
	scaleOffset = vec3(0);
	positionOffset = vec3(0);
	rotationOffset = quat();
	mesh = referenced;
}


EMultiDrawMeshInstance::~EMultiDrawMeshInstance()
{
}
