#pragma once
#include <EEngine.h>
#define STB_IMAGE_IMPLEMENTATION
#include <GLFW\glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

using namespace std;
using namespace glm;

class Texture
{
public:
	DllExport Texture();
	unsigned int handle;
	int layer;
	~Texture();
	unsigned int id;
	string type;
	aiString path;
};
