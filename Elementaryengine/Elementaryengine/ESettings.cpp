#include "stdafx.h"
#include "ESettings.h"


ESettings::ESettings()
{
	fullscreen = false;
	windowHeight = 600;
	windowWidth = 800;
	blankwindow = false;
	name = "Elementaryengine";
	vsync = false;

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = name.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Elementaryengine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.apiVersion = VK_API_VERSION_1_0;
}


ESettings::~ESettings()
{
}
