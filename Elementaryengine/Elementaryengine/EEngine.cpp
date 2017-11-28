#include "stdafx.h"
#include "EEngine.h"
#include "EEngineInitialise.h"

EEngine::EEngine()
{
	activeSettings = new ESettings();
}


EEngine::~EEngine()
{
}

void EEngine::start()
{
	EEngineInitialise::init(activeSettings, this);
	loop();
}

void EEngine::loop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	EEngineInitialise::cleanup(this);
}
