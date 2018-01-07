#pragma once
#include "Asset.h"
#include <Feather.h>
#include <Shader.h>
#include <Texture.h>
#include <Model.h>
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
class Lamp :
	public AssetComponent
{
public:
	DllExport Lamp();
	DllExport ~Lamp();
	DllExport void Render(mat4 view, mat4 projection);
	Model* model;
	DllExport static void SetupLampComp();
	vec3 color;
	vec3 Direction;
	static unsigned int depthMapFBO;
	Texture* depthmap;
	static const unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
	bool throwShadows;
};

