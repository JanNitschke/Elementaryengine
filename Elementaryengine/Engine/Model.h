#pragma once
#include <Feather.h>
#include <Shader.h>
#include <Mesh.h>
#include <AssetComponent.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Texture.h>
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

using namespace std;
class DllExport Model :
	public AssetComponent
{
public:
	Model();
	~Model();
	Model(char *path);
	vector<Mesh*> meshes;

	void Render(mat4 view, mat4 projection, Asset* parent, Shader* s);
	void RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a);

private:
	/*  Model Data  */
	string directory;
	/*  Functions   */
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

