#pragma once
#include <Material.h>
#include <glm\glm.hpp>
#include <Shader.h>
#include <Feather.h>
#include <Mesh.h>

using namespace glm;

void eDrawCommand(unsigned int VAO, unsigned int EBO, vector<unsigned int> indices);

void eSetMat4fCommand(Shader* , mat4 , const char* );

void eSetVec3fCommand(Shader* , vec3 , const char* );

void eSetFCommand(Shader*, float , const char* );

void eSetMaterialCommand(Shader*, PBRMaterial*);

void eSetLampsCommand(Shader*, GLuint, GLuint);
