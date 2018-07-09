#include "EMeshReference.h"




EMeshReference::EMeshReference(Mesh * referenced)
{
	mesh = referenced;
}

EMeshReference::~EMeshReference()
{
}

Material * EMeshReference::getMaterial()
{
	return mesh->material;
}
