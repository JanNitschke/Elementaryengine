#pragma once
#include <Asset.h>
#include <Feather.h>
#include <Shader.h>
#include <Texture.h>
#include <Material.h>

using namespace glm;

struct Vertex {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
};


class DllExport Mesh :
	public AssetComponent
{
public:
	vec3 posOffset;
	vec3 rotOffset;
	vec3 scaleOffset;
	vector<Asset*> parents;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	Mesh();
	Mesh( vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture*> textures);
	~Mesh();
	virtual void Render(mat4 view, mat4 projection, Asset* parent, Shader* s);
	virtual void RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a);
	Material* material;
	GLuint VertexArrayID;
	GLuint vertexbuffer;
	static Texture* colorCorrection;
	static Shader* defaultShader;
	static Shader* lightmapShader;
	static Shader* pbrShader;
	static Shader* geometryShader;
	static Shader* voxelShader;
	static Shader* terrainShader;
	static Shader* terrainLightmapShader;
	static Shader* terrainEnvShader;
	static Shader* grassShader;
	static Shader* ssrShader;

	static void SetupMeshComp();
	void SetupPbrUniforms(mat4 model, PBRMaterial* mat, Shader* shader);

	unsigned int VAO, VBO, EBO;
	GLuint VPMatrixID;
	GLuint ModelMatrixID;
	void SetupMesh();
	mat4 Model();
};

