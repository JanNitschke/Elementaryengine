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
Shader* Mesh::pbrEnvShader;
Shader* Mesh::terrainEnvShader;
Shader* Mesh::terrainLightmapShader;
Shader* Mesh::terrainShader;
Shader* Mesh::grassShader;
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
}


Mesh::~Mesh()
{
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}

void Mesh::SetupMeshComp()
{
	defaultShader = new Shader("..\\shaders\\DefaultShader.vert", "..\\shaders\\DefaultShader.frag");
	lightmapShader = new Shader("..\\shaders\\LightmapShader.vert", "..\\shaders\\LightmapShader.geom","..\\shaders\\LightmapShader.frag");
	pbrShader = new Shader("..\\shaders\\DefaultShader.vert", "..\\shaders\\PBRShader.frag");
	pbrEnvShader = new Shader("..\\shaders\\EnvShader.vert", "..\\shaders\\EnvShader.geom", "..\\shaders\\PBRShader.frag");
	grassShader = new Shader("..\\shaders\\TerrainShader.vert","..\\shaders\\Grass.geom" , "..\\shaders\\Grass.frag");
	terrainShader = new Shader("..\\shaders\\TerrainShader.vert", "..\\shaders\\TerrainShader.geom", "..\\shaders\\PBRShader.frag");
	//terrainLightmapShader = new Shader("..\\shaders\\TerrainLightmapShader.vert", "..\\shaders\\LightmapShader.geom", "..\\shaders\\LightmapShader.frag");
	//terrainEnvShader = new Shader("..\\shaders\\TerrainEnvShader.vert", "..\\shaders\\TerrainEnvShader.geom", "..\\shaders\\PBRShader.frag");
	colorCorrection = new Texture("Assets/Textures/colorCorrection.jpg");
}

void Mesh::SetupMesh()
{
	VPMatrixID = glGetUniformLocation(Mesh::defaultShader->ID, "Model");
	ModelMatrixID = glGetUniformLocation(Mesh::defaultShader->ID, "VP");

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


	glGenBuffers(1, &lightColorSSBO);
	glGenBuffers(1, &lightPositionSSBO);

	glBindVertexArray(0);
	/**/
}


void Mesh::Render(mat4 view, mat4 projection, Asset* parent)
{
	Shader* shader;
	mat4 Model = mat4(1.0f);
	Model = translate(Model, parent->position + posOffset);
	Model = glm::scale(Model, parent->scale + scaleOffset);
	quat q = parent->q;
	mat4 rotate = glm::toMat4(q);
	Model = Model;
	mat4 VP = projection * view;
	if (DefaultMaterial* mat = dynamic_cast<DefaultMaterial*>(material)) {
		shader = defaultShader;
		shader->use();
		shader->setMat4f("Model", Model);
		shader->setMat4f("VP", VP);

		shader->set3Float("directionalLightDirection", Game::directionalLightDirection);
		shader->set3Float("directionalLightColor", Game::directionalLightColor);
		shader->set3Float("viewPos", Game::activeCam->position);

		//Material loading
		shader->set3Float("material.ambient", mat->ambient);
		shader->set3Float("material.diffuse", mat->diffuse);
		shader->set3Float("material.specular", mat->specular);
		shader->setFloat("material.shininess", mat->shininess);
		shader->set2Float("TextureCoordinateMultiply", material->TextureScale);
		shader->setFloat("far_plane", 25);

		if (mat->diffuseMap != nullptr) {
			shader->setInt("material.Texture", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mat->diffuseMap->id);
		}
		if (mat->specularMap != nullptr) {
			shader->setInt("material.specularMap", 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mat->specularMap->id);
		}

		if (mat->ambientMap != nullptr) {
			shader->setInt("material.ambientMap", 3);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mat->ambientMap->id);
		}

	}
	if (PBRMaterial* mat = dynamic_cast<PBRMaterial*>(material)) {
		shader = pbrShader;
		SetupPbrUniforms(Model,mat,shader);
		shader->setMat4f("VP", VP);
		shader->setInt("renderRef", 1);

		shader->setInt("envMap", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, parent->environmentMap->id);

		shader->setInt("colorCorrection", 6);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, Mesh::colorCorrection->id);
	}
	shader->setMat4f("Rot",rotate);

	vector<vec4> lightColors;
	vector<vec4> lightPositions;
	int i = 0;
	for each (Lamp* l in Game::lamps) {
		vec3 outcol = l->color;
		vec3 outpos = l->parent->position;
		lightColors.push_back(vec4(outcol,0));
		lightPositions.push_back(vec4(outpos,0));
		glActiveTexture(GL_TEXTURE8 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, l->depthmap->id);
		string str = "depthMaps[";
		str.append(std::to_string(i));
		str.append("]");
		shader->setInt(str,i + 8);
		i++;
	}
	//Color SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
	GLuint block_index_c = 0;
	block_index_c = glGetProgramResourceIndex(shader->ID, GL_SHADER_STORAGE_BLOCK, "lightColors");
	GLuint ssbo_binding_point_index_c = 3;
	glShaderStorageBlockBinding(shader->ID, block_index_c, ssbo_binding_point_index_c);
	GLsizeiptr lcs = sizeof(glm::vec4) * (lightColors.size());
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_c, lightColorSSBO);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_c, lightColorSSBO, 0, lcs);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lcs, lightColors.data(), GL_DYNAMIC_DRAW);

	//position SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
	GLuint block_index_p = 0;
	block_index_p = glGetProgramResourceIndex(shader->ID, GL_SHADER_STORAGE_BLOCK, "lightPositions");
	GLuint ssbo_binding_point_index_p = 4;
	glShaderStorageBlockBinding(shader->ID, block_index_p, ssbo_binding_point_index_p);
	GLsizeiptr lps = sizeof(glm::vec4) * lightPositions.size();
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_p, lightPositionSSBO);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_p, lightPositionSSBO, 0, lps);

	glBufferData(GL_SHADER_STORAGE_BUFFER, lps, lightPositions.data(), GL_DYNAMIC_DRAW);


	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a)
{
	Shader* shader = lightmapShader;
	mat4 Model(1.0f);
	Model = translate(Model, a->position + posOffset);
	Model = scale(Model, a->scale + scaleOffset);
	Model = Model * glm::toMat4(a->q);;

	shader->use();
	shader->setMat4f("Model", Model);

	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "shadowMatrices"), view.size(), GL_FALSE, glm::value_ptr(view[0]));
	shader->setFloat("far_plane", 25);
	shader->set3Float("lightPos", l->parent->position);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::RenderEnvMap(vector<mat4> view, mat4 projection, Asset* a)
{
	if (PBRMaterial* mat = dynamic_cast<PBRMaterial*>(material)) {
		Shader* shader = pbrEnvShader;
		mat4 Model(1.0f);
		Model = translate(Model, a->position + posOffset);
		Model = scale(Model, a->scale + scaleOffset);
		Model = Model * glm::toMat4(a->q);;

		SetupPbrUniforms(Model, mat, shader);
		shader->setInt("renderRef", 0);
		glUniformMatrix4fv(glGetUniformLocation(shader->ID, "envMatrices"), view.size(), GL_FALSE, glm::value_ptr(view[0]));


		vector<vec4> lightColors;
		vector<vec4> lightPositions;
		int i = 0;
		for each (Lamp* l in Game::lamps) {
			vec3 outcol = l->color;
			vec3 outpos = l->parent->position;
			lightColors.push_back(vec4(outcol, 0));
			lightPositions.push_back(vec4(outpos, 0));
			glActiveTexture(GL_TEXTURE7 + i);
			glBindTexture(GL_TEXTURE_CUBE_MAP, l->depthmap->id);
			string str = "depthMaps[";
			str.append(std::to_string(i));
			str.append("]");
			shader->setInt(str, i + 7);
			i++;
		}
		//Color SSBO
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
		GLuint block_index_c = 0;
		block_index_c = glGetProgramResourceIndex(shader->ID, GL_SHADER_STORAGE_BLOCK, "lightColors");
		GLuint ssbo_binding_point_index_c = 3;
		glShaderStorageBlockBinding(shader->ID, block_index_c, ssbo_binding_point_index_c);
		GLsizeiptr lcs = sizeof(glm::vec4) * (lightColors.size());
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_c, lightColorSSBO);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_c, lightColorSSBO, 0, lcs);
		glBufferData(GL_SHADER_STORAGE_BUFFER, lcs, lightColors.data(), GL_DYNAMIC_DRAW);

		//position SSBO
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
		GLuint block_index_p = 0;
		block_index_p = glGetProgramResourceIndex(shader->ID, GL_SHADER_STORAGE_BLOCK, "lightPositions");
		GLuint ssbo_binding_point_index_p = 4;
		glShaderStorageBlockBinding(shader->ID, block_index_p, ssbo_binding_point_index_p);
		GLsizeiptr lps = sizeof(glm::vec4) * lightPositions.size();
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_p, lightPositionSSBO);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_p, lightPositionSSBO, 0, lps);

		glBufferData(GL_SHADER_STORAGE_BUFFER, lps, lightPositions.data(), GL_DYNAMIC_DRAW);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
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
