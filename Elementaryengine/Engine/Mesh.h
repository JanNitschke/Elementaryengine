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
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	Mesh();
	Mesh( vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture*> textures);
	~Mesh();

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

	unsigned int VAO, VBO, EBO;
	GLuint VPMatrixID;
	GLuint ModelMatrixID;
	void SetupMesh();
	mat4 Model();
};

