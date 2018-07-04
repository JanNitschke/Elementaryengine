#include "EMeshReference.h"




EMeshReference::EMeshReference(Mesh * referenced)
{
	scaleOffset = vec3(0);
	positionOffset = vec3(0);
	rotationOffset = quat();
	mesh = referenced;
}

EMeshReference::~EMeshReference()
{
}

Material * EMeshReference::getMaterial()
{
	return mesh->material;
}
