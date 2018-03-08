#include "Terrain.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Lamp.h>
#include <Game.h>
using namespace glm;
Terrain::Terrain()
{
	//SetupTerrain();
}

Terrain::Terrain(Mesh* m, Texture* t)
{
	vertices = m->vertices;
	indices = m->indices;
	heightmap = t;
	SetupTerrain();
}

Terrain::~Terrain()
{
}

void Terrain::SetupTerrain()
{
	if (!Game::isServer) {
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



		glBindVertexArray(0);
	}
}

void Terrain::Render(mat4 view, mat4 projection, Asset * parent)
{
	//Shader* shader;
	//mat4 Model = mat4(1.0f);
	//Model = translate(Model, parent->position + posOffset);
	//Model = glm::scale(Model, parent->scale + scaleOffset);
	//quat q = parent->q;
	//mat4 rotate = glm::toMat4(q);
	//Model = Model;
	//mat4 VP = projection * view;
	//
	//if (PBRMaterial* mat = dynamic_cast<PBRMaterial*>(material)) {
	//	shader = Mesh::terrainShader;
	//	SetupPbrUniforms(Model, mat, shader);
	//	shader->setMat4f("VP", VP);
	//	shader->setInt("renderRef", 0);

	//	shader->setInt("heightmap", 5);
	//	glActiveTexture(GL_TEXTURE5);
	//	glBindTexture(GL_TEXTURE_2D, parent->environmentMap->id);
	//}
	//shader->setMat4f("Rot", rotate);

	//vector<vec4> lightColors;
	//vector<vec4> lightPositions;
	//int i = 0;
	//for each (Lamp* l in Game::lamps) {
	//	vec3 outcol = l->color;
	//	vec3 outpos = l->parent->position;
	//	lightColors.push_back(vec4(outcol, 0));
	//	lightPositions.push_back(vec4(outpos, 0));
	//	glActiveTexture(GL_TEXTURE7 + i);
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, l->depthmap->id);
	//	string str = "depthMaps[";
	//	str.append(std::to_string(i));
	//	str.append("]");
	//	shader->setInt(str, i + 7);
	//	i++;
	//}
	//if (heightmap != nullptr) {
	//	shader->setInt("heightmap", 5);
	//	glActiveTexture(GL_TEXTURE5);
	//	glBindTexture(GL_TEXTURE_2D, heightmap->id);
	//}
	//shader->setInt("colorCorrection", 6);
	//glActiveTexture(GL_TEXTURE6);
	//glBindTexture(GL_TEXTURE_2D, Mesh::colorCorrection->id);
	////Color SSBO
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
	//GLuint block_index_c = 0;
	//block_index_c = glGetProgramResourceIndex(shader->ID, GL_SHADER_STORAGE_BLOCK, "lightColors");
	//GLuint ssbo_binding_point_index_c = 3;
	//glShaderStorageBlockBinding(shader->ID, block_index_c, ssbo_binding_point_index_c);
	//GLsizeiptr lcs = sizeof(glm::vec4) * (lightColors.size());
	////glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_c, lightColorSSBO);
	//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_c, lightColorSSBO, 0, lcs);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, lcs, lightColors.data(), GL_DYNAMIC_DRAW);

	////position SSBO
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
	//GLuint block_index_p = 0;
	//block_index_p = glGetProgramResourceIndex(shader->ID, GL_SHADER_STORAGE_BLOCK, "lightPositions");
	//GLuint ssbo_binding_point_index_p = 4;
	//glShaderStorageBlockBinding(shader->ID, block_index_p, ssbo_binding_point_index_p);
	//GLsizeiptr lps = sizeof(glm::vec4) * lightPositions.size();
	////glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_p, lightPositionSSBO);
	//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_p, lightPositionSSBO, 0, lps);

	//glBufferData(GL_SHADER_STORAGE_BUFFER, lps, lightPositions.data(), GL_DYNAMIC_DRAW);
	//glDisable(GL_CULL_FACE);

	//glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
	//glEnable(GL_CULL_FACE);

	////Grass

	//shader = Mesh::grassShader;
	//shader->use();
	//shader->setMat4f("Model", Model);
	//shader->set3Float("directionalLightDirection", Game::directionalLightDirection);
	//shader->set3Float("directionalLightColor", Game::directionalLightColor);
	//shader->set3Float("viewPos", Game::activeCam->position);
	//shader->setMat4f("VP", VP);
	//shader->setInt("renderRef", 0);
	//shader->setFloat("far_plane", 25);

	//
	//shader->setMat4f("Rot", rotate);

	//shader->set3Float("grassColor", vec3(0.290, 0.411, 0.086));
	//shader->set3Float("grassBorderColor",vec3(0.450, 0.674, 0.086));
	//shader->setFloat("timeRunning", glfwGetTime());

	//shader->setInt("colorCorrection", 4);
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, Mesh::colorCorrection->id);

	//int j = 0;
	//for each (Lamp* l in Game::lamps) {
	//	vec3 outcol = l->color;
	//	vec3 outpos = l->parent->position;
	//	lightColors.push_back(vec4(outcol, 0));
	//	lightPositions.push_back(vec4(outpos, 0));
	//	glActiveTexture(GL_TEXTURE7 + j);
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, l->depthmap->id);
	//	string str = "depthMaps[";
	//	str.append(std::to_string(i));
	//	str.append("]");
	//	shader->setInt(str, j + 7);
	//	i++;
	//}
	//if (heightmap != nullptr) {
	//	shader->setInt("heightmap", 5);
	//	glActiveTexture(GL_TEXTURE5);
	//	glBindTexture(GL_TEXTURE_2D, heightmap->id);
	//}
	//if (grassMap != nullptr) {
	//	shader->setInt("grassMap", 6);
	//	glActiveTexture(GL_TEXTURE6);
	//	glBindTexture(GL_TEXTURE_2D, grassMap->id);
	//}
	////Color SSBO
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
	//block_index_c = glGetProgramResourceIndex(shader->ID, GL_SHADER_STORAGE_BLOCK, "lightColors");
	//glShaderStorageBlockBinding(shader->ID, block_index_c, ssbo_binding_point_index_c);
	////glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_c, lightColorSSBO);
	//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_c, lightColorSSBO, 0, lcs);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, lcs, lightColors.data(), GL_DYNAMIC_DRAW);

	////position SSBO
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
	//block_index_p = glGetProgramResourceIndex(shader->ID, GL_SHADER_STORAGE_BLOCK, "lightPositions");
	//glShaderStorageBlockBinding(shader->ID, block_index_p, ssbo_binding_point_index_p);
	////glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_p, lightPositionSSBO);
	//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index_p, lightPositionSSBO, 0, lps);

	//glBufferData(GL_SHADER_STORAGE_BUFFER, lps, lightPositions.data(), GL_DYNAMIC_DRAW);
	//glDisable(GL_CULL_FACE);

	//glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
	//glEnable(GL_CULL_FACE);

}

void Terrain::RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent * l, Asset * a)
{
}

void Terrain::RenderEnvMap(vector<mat4> view, mat4 projection, Asset * a)
{
}
