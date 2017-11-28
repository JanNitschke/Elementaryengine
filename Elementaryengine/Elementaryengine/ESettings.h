#pragma once
#include <string>
#include <vulkan\vulkan.h>
using namespace std;
class ESettings
{
public:
	ESettings();
	~ESettings();
	bool fullscreen;
	bool blankwindow;
	bool vsync;
	string name;	
	int windowWidth;
	int windowHeight;
	VkApplicationInfo appInfo = {};

};

