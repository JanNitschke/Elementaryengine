#include "ERenderCommand.h"
#include <Mesh.h>
#include <Game.h>
#include "iostream"

void eSetMaterialCommand(Shader * shader, PBRMaterial * material)
{

	PBRMaterial* mat = material;

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



	shader->setInt("renderRef", 1);
	shader->setInt("envMap", 5);



	//shader->setInt("colorCorrection", 6);
	//glActiveTexture(GL_TEXTURE5);
	//glBindTexture(GL_TEXTURE_2D, Mesh::colorCorrection->id);


}

void eSetLampsCommand(Shader * shader, GLuint lightColorSSBO, GLuint lightPositionSSBO)
{
	vector<vec4> lightColors;
	vector<vec4> lightPositions;
	int i = 0;
	for each (Lamp* l in Game::lamps) {
		vec3 outcol = l->color;
		vec3 outpos = l->parent->position;
		lightColors.push_back(vec4(outcol, 0));
		lightPositions.push_back(vec4(outpos, 0));
		glActiveTexture(GL_TEXTURE8 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, l->depthmap->id);
		string str = "depthMaps[";
		str.append(std::to_string(i));
		str.append("]");
		shader->setInt(str, i + 8);
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
}

void eDrawCommand(unsigned int VAO, unsigned int EBO, vector<unsigned int> indices)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void eSetFCommand(Shader * shader, float value, const char * name)
{
	shader->setFloat(name, value);
}

void eSetVec3fCommand(Shader * shader, vec3 value, const char * name)
{
	shader->set3Float(name, value);
}

void eSetMat4fCommand(Shader * shader, mat4 value, const char * name)
{
	shader->setMat4f(name, value);
}
