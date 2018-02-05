#pragma once
#include "Camera.h"
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
class FPCam :
	public Camera
{
public:
	DllExport FPCam();
	DllExport ~FPCam();
	mat4 GetView();
	void Tick(GLFWwindow * window, double deltaTime);
	float camSpeed = 2.0f;

private:
	float lastX, lastY;
	bool firstMouse = true;

};

