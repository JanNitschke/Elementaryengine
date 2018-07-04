#pragma once
#include <Mesh.h>
#include <AssetComponent.h>

using namespace std;
using namespace glm;

class EMultiDrawMeshInstance:
	public AssetComponent
{
public:	
	bool visible;
	vec3 positionOffset;
	vec3 scaleOffset;
	quat rotationOffset;
	Mesh * mesh;
	virtual void setPosition(vec3 pos);
	virtual void setRotation(quat rot);
	virtual void setScale(vec3 sca);

	EMultiDrawMeshInstance(Mesh * referenced);
	~EMultiDrawMeshInstance();
};

