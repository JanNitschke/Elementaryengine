#pragma once
#include <Mesh.h>
#include <EEngine.h>
#include <Camera.h>
#include <Asset.h>
#include <Lamp.h>
#include <EOpenGl.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "UIElement.h"

class ERenderer
{
public:

	virtual void SetupFrame(bool meshChanged, EOpenGl* eOpenGl) = 0;
	virtual void RenderFrame(EOpenGl* eOpenGl, EDisplaySettings* displaySettings, mat4 View, mat4 Projection) = 0;
	virtual void RenderFX(EOpenGl* eOpenGl, EDisplaySettings* displaySettings) = 0;
	virtual void Setup(EOpenGl* eOpenGl, EDisplaySettings* displaySettings) = 0;
	virtual Texture* loadTexture(const char* path) = 0;

	static void AssetCreatedCallback(Asset* asset);
	static void AssetChangedCallback(Asset* asset);
	static void AssetDestroyedCallback(Asset* asset);
};

