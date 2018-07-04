#pragma once
#include <Mesh.h>
#include <AssetComponent.h>

using namespace std;
using namespace glm;

class EMeshReference :
	public AssetComponent
{
public:

	vec3 positionOffset;
	vec3 scaleOffset;
	quat rotationOffset;
	Mesh * mesh;

	EMeshReference(Mesh * referenced);
	~EMeshReference();

	Material * getMaterial();
};

