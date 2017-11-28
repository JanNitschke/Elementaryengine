#include "stdafx.h"
#include "EEngineInitialise.h"
#include <vector>

using namespace std;


void EEngineInitialise::init(ESettings * eSettings, EEngine* engine)
{
	// glfw

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (eSettings->blankwindow && eSettings->fullscreen) {
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		eSettings->windowHeight = mode->height;
		eSettings->windowWidth = mode->width;
	}
	else {
		glfwWindowHint(GLFW_DECORATED, (eSettings->blankwindow) ? GLFW_FALSE : GLFW_TRUE);

	}

	engine->window = glfwCreateWindow(eSettings->windowWidth, eSettings->windowHeight, eSettings->name.c_str(), (eSettings->fullscreen) ? glfwGetPrimaryMonitor() : nullptr, nullptr);

	// Vulkan

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &eSettings->appInfo;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &engine->instance);

	if (vkCreateInstance(&createInfo, nullptr, &engine->instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}

	// Vulkan extentions

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "available extensions:" << std::endl;

	for (const auto& extension : extensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}

}

void EEngineInitialise::cleanup(EEngine* engine)
{
	// Vulkan
	vkDestroyInstance(engine->instance, nullptr);

	// GLFW
	glfwDestroyWindow(engine->window);

	glfwTerminate();
}

EEngineInitialise::EEngineInitialise()
{
}


EEngineInitialise::~EEngineInitialise()
{
}

bool checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	return false;
}
