#pragma once
#include "Asset.h"
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

using namespace glm;
class Camera :
	public Asset
{
public:
	DllExport Camera();
	DllExport ~Camera();
	virtual mat4 GetView();
	vec3 cameraUp;
	float camX, camZ;
	vec3 cameraFront;
	void Tick(GLFWwindow * window, double deltaTime);
};

