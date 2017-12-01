#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include "EEngine.h"

class EEngineInitialise
{
public:
	static void init(ESettings* eSettings, EEngine* engine);
	static void cleanup(EEngine* engine);
private:
	EEngineInitialise();
	~EEngineInitialise();
};

struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0;
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

bool checkValidationLayerSupport();
bool isDeviceSuitable(VkPhysicalDevice device);
bool checkDeviceExtensionSupport(VkPhysicalDevice device); 
void pickPhysicalDevice(EEngine* engine);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
void createLogicalDevice(EEngine* engine);
void createSurface(EEngine* engine);
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, EEngine* engine);