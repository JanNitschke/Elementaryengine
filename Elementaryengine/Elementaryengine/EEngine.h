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

	// GLFW and vulkan stuff
	GLFWwindow* window;
	VkInstance instance;
	VkDevice device;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkQueue presentQueue;
	VkDebugReportCallbackEXT callback;

	ESettings* activeSettings;
};

