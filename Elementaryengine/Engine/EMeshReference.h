#pragma once
#include <Mesh.h>
#include <AssetComponent.h>

using namespace std;
using namespace glm;

class EMeshReference :
	public AssetComponent
{
public:

	vec3 positionOffset = vec3(0);
	vec3 scaleOffset = vec3(1);
	quat rotationOffset = quat();
	Mesh * mesh;

	EMeshReference(Mesh * referenced);
	~EMeshReference();

	Material * getMaterial();
};

