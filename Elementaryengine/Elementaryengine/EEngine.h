#pragma once
#include <GLFW\glfw3.h>
#include "ESettings.h"

class EEngine
{
public:
	EEngine();
	~EEngine();
	void start();
	void loop();
	GLFWwindow* window;
	VkInstance instance;
	ESettings* activeSettings;
	VkDebugReportCallbackEXT callback;
};

