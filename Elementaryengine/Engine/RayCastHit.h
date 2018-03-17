#pragma once
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
#include <EEngine.h>

using namespace glm;

///<summary>
///Game class abstracts all things that belong to the game. Run Start() after creating an instance to run the game. Do not forget to set the window options and gamemode first.
///You can not create more than one instance of this class;
///</summary> 
struct DllExport RayCastHit {
	Asset* hitAsset;
	vec3 hitPos;
	vec3 hitNormal;
};