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
#include <ERenderCommand.h>

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
}


Mesh::~Mesh()
{
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


void Mesh::Render(mat4 view, mat4 projection, Asset* parent, Shader* s)
{
	Shader* shader = s;
	mat4 Model = mat4(1.0f);
	Model = translate(Model, parent->position + posOffset);
	Model = glm::scale(Model, parent->scale + scaleOffset);
	quat q = parent->q;
	mat4 rotate = glm::toMat4(q);
	Model = Model;
	mat4 VP = projection * view;

	Game* g = &Game::Instance();
	EOpenGl* eOpenGl = g->eOpenGl;

	eSetMaterialCommand(shader, dynamic_cast<PBRMaterial*>(material));
	eSetMat4fCommand(shader, VP, "VP");
	eSetMat4fCommand(shader, rotate, "Rot");
	eSetMat4fCommand(shader, Model, "Model");
	eSetMat4fCommand(shader, Game::Instance().VoxelProj, "voxelprojection");
	eDrawCommand(VAO,EBO,indices);

}

void Mesh::RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a)
{
	Shader* shader = lightmapShader;
	mat4 Model(1.0f);
	Model = translate(Model, a->position + posOffset);
	Model = scale(Model, a->scale + scaleOffset);
	Model = Model * glm::toMat4(a->q);;

	shader->setMat4f("Model", Model);

	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "shadowMatrices"), view.size(), GL_FALSE, glm::value_ptr(view[0]));
	shader->setFloat("far_plane", 25);
	shader->set3Float("lightPos", l->parents[0]->position);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupPbrUniforms(mat4 model, PBRMaterial* mat, Shader* shader)
{
	shader->use();
	shader->setMat4f("Model", model);

	shader->set3Float("directionalLightDirection", Game::directionalLightDirection);
	shader->set3Float("directionalLightColor", Game::directionalLightColor);
	shader->set3Float("viewPos", Game::activeCam->position);



	//Material loading
	shader->set3Float("material.albedo", mat->albedo);
	shader->setFloat("material.roughness", mat->roughness);
	shader->set3Float("material.ao", mat->ao);
	shader->setFloat("material.metallic", mat->metallic);

	shader->set2Float("TextureCoordinateMultiply", material->TextureScale);
	shader->setFloat("far_plane", 25);

	if (mat->albedoMap != nullptr) {
		shader->setInt("material.albedoMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mat->albedoMap->id);
	}
	if (mat->roughnessMap != nullptr) {
		shader->setInt("material.roughnessMap", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mat->roughnessMap->id);
	}

	if (mat->metallicMap != nullptr) {
		shader->setInt("material.metallicMap", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mat->metallicMap->id);
	}
	if (mat->normalMap != nullptr) {
		shader->setInt("material.normalMap", 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mat->normalMap->id);
	}
	if (mat->aoMap != nullptr) {
		shader->setInt("material.aoMap", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, mat->aoMap->id);
	}
}
