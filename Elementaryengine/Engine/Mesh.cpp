#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Shader.h>
#include <Game.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <GLFW\glfw3.h>
#include <Lamp.h>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Shader* Mesh::defaultShader;
Shader* Mesh::lightmapShader;
Shader* Mesh::pbrShader;
Shader* Mesh::geometryShader;
Shader* Mesh::voxelShader;
Shader* Mesh::terrainEnvShader;
Shader* Mesh::terrainLightmapShader;
Shader* Mesh::terrainShader;
Shader* Mesh::grassShader;
Shader* Mesh::ssrShader;
Shader* Mesh::uiShader;

Texture* Mesh::colorCorrection;

Mesh::Mesh()
{
}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture*> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	if (!Game::isServer) {
		SetupMesh();
	}
	Game::meshs.push_back(this);
	Game::meshChanged = true;
}


Mesh::~Mesh()
{
	Game::meshChanged = true;
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}

void Mesh::SetupMeshComp()
{
	//defaultShader = new Shader("..\\shaders\\DefaultShader.vert", "..\\shaders\\DefaultShader.frag");
	lightmapShader = new Shader("..\\shaders\\LightmapShader.vert", "..\\shaders\\LightmapShader.geom","..\\shaders\\LightmapShader.frag");
	pbrShader = new Shader("..\\shaders\\geometry.vert", "..\\shaders\\PBRShader.frag");
	ssrShader = new Shader("..\\shaders\\geometry.vert", "..\\shaders\\SSR.frag");
	geometryShader = new Shader("..\\shaders\\DefaultShader.vert", "..\\shaders\\geometry.frag");
	//uiShader = new Shader("..\\shaders\\UI.vert", "..\\shaders\\UI.frag");

	//voxelShader = new Shader("..\\shaders\\voxelise.vert", "..\\shaders\\voxelise.geom", "..\\shaders\\voxelise.frag");
	//grassShader = new Shader("..\\shaders\\TerrainShader.vert","..\\shaders\\Grass.geom" , "..\\shaders\\Grass.frag");
	//terrainShader = new Shader("..\\shaders\\TerrainShader.vert", "..\\shaders\\TerrainShader.geom", "..\\shaders\\PBRShader.frag");
	//terrainLightmapShader = new Shader("..\\shaders\\TerrainLightmapShader.vert", "..\\shaders\\LightmapShader.geom", "..\\shaders\\LightmapShader.frag");
	//terrainEnvShader = new Shader("..\\shaders\\TerrainEnvShader.vert", "..\\shaders\\TerrainEnvShader.geom", "..\\shaders\\PBRShader.frag");
	colorCorrection = new Texture("Assets/Textures/colorCorrection.jpg");
}

void Mesh::SetupMesh()
{
	//VPMatrixID = glGetUniformLocation(Mesh::defaultShader->ID, "Model");
	//ModelMatrixID = glGetUniformLocation(Mesh::defaultShader->ID, "VP");

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));



	glBindVertexArray(0);
	/**/
}

mat4 Mesh::Model()
{
	mat4 model = mat4(1.0f);
	model = translate(model, parents[0]->position + posOffset);
	model = glm::scale(model, parents[0]->scale + scaleOffset);
	return model;
}
