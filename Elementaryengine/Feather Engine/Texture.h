#pragma once
#include <Feather.h>
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
	DllExport Texture(char path);
	DllExport Texture();
	// 1 = envmap

	DllExport Texture(int type);
	DllExport Texture(const char* path);
	void Load(const char* path);
	~Texture();
	unsigned int id;
	string type;
	aiString path;
	static unsigned char* LoadPicture(const char* path);
};
