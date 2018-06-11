#pragma once
#include <EEngine.h>
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
	vector<Material*> materials;

private:
	/*  Model Data  */
	string directory;
	/*  Functions   */
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

