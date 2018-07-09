#include "EGeometryPass.h"
#include <EOpenGL.h>
#include <Game.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <EModularRasterizer.h>
#include <iostream>

Shader * EGeometryPass::meshGeometryShader;

EGeometryPass::EGeometryPass()
{
	ERenderPass::Initialize();

	GLenum err = 0;

	framebuffer = new EOGLFramebuffer();
	framebuffer->Bind();

	EOGLBindlessTexture* positionBuffer = new EOGLBindlessTexture("positionBuffer", GL_TEXTURE_2D, GL_NEAREST, GL_RGB16F, GL_RGB, GL_FLOAT, displaySettings->windowWidth, displaySettings->windowHeight);
	framebuffer->AppendTexture(positionBuffer, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);

	EOGLBindlessTexture* normalBuffer = new EOGLBindlessTexture("normalBuffer", GL_TEXTURE_2D, GL_NEAREST, GL_RGB16F, GL_RGB, GL_FLOAT, displaySettings->windowWidth, displaySettings->windowHeight);
	framebuffer->AppendTexture(normalBuffer, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D);

	EOGLBindlessTexture* albedoSpecBuffer = new EOGLBindlessTexture("albedoSpecBuffer", GL_TEXTURE_2D, GL_NEAREST, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, displaySettings->windowWidth, displaySettings->windowHeight);
	framebuffer->AppendTexture(albedoSpecBuffer, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D);

	EOGLBindlessTexture* materialBuffer = new EOGLBindlessTexture("materialBuffer", GL_TEXTURE_2D, GL_NEAREST, GL_RGB16F, GL_RGB, GL_FLOAT, displaySettings->windowWidth, displaySettings->windowHeight);
	framebuffer->AppendTexture(materialBuffer, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D);

	EOGLBindlessTexture* depthBuffer = new EOGLBindlessTexture("depthBuffer", GL_TEXTURE_2D, GL_LINEAR, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, displaySettings->windowWidth, displaySettings->windowHeight);
	framebuffer->AppendTexture(depthBuffer, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D);

	framebuffer->Complete();
}


EGeometryPass::~EGeometryPass()
{
}

void EGeometryPass::Render()
{
	GLenum err = 0;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error before geom: " << err << endl;
	}
	ERenderPass::Render();

	EOpenGl* eOpenGl = Game::eOpenGl;
	framebuffer->Bind();
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error geom framebuffer: " << err << endl;
	}

	glDisable(GL_BLEND);
	glClearColor(0.050f, 0.125f, 0.247f, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//set viewport
	glViewport(0, 0, displaySettings->windowWidth, displaySettings->windowHeight);

	meshGeometryShader->use();
	meshUniformVP.Update();

	for each (auto level in Game::levels) {
		if (level->isLoaded()) {
			for each(auto asset in level->assets) {
				for each (AssetComponent* component in asset->components)
				{
					if ((dynamic_cast<EMeshReference*>(component) != nullptr)) {
						RenderMesh((EMeshReference*)component);
					}
				}
				if ((dynamic_cast<EMultiDrawContainer*>(asset) != nullptr)) {
					RenderMultiDraw((EMultiDrawContainer*)asset);
				}
			}
		}
	}

	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error geom end: " << err << endl;
	}
}

void EGeometryPass::Initialize()
{
	meshGeometryShader = new Shader("..\\shaders\\GeometryPassMesh.vert", "..\\shaders\\GeometryPass.frag");
	_shader = meshGeometryShader;
	_shader->use();
	meshUniformVP = EOGLUniform<mat4>(meshGeometryShader, "VP", []() { return Game::Projection * Game::View; });
	meshUniformModel = EOGLUniform<mat4>(meshGeometryShader, "Model", mat4(0));
	meshUniformRotation = EOGLUniform<mat4>(meshGeometryShader, "Rot", mat4(0));
	meshUniformAO = EOGLUniform<vec3>(meshGeometryShader, "ao", vec3(0));
	meshUniformAlbedo = EOGLUniform<vec3>(meshGeometryShader, "albedo", vec3(0));
	meshUniformRoughness = EOGLUniform<float>(meshGeometryShader, "roughness", 0.0f);
	meshUniformMetallic = EOGLUniform<float>(meshGeometryShader, "metallic", 0.0f);
	meshUniformRoughnessTex = EOGLUniform<int>(meshGeometryShader, "roughnessTex", 0);
	meshUniformAlbedoTex = EOGLUniform<int>(meshGeometryShader, "albedoTex", 0);
	meshUniformMetallicTex = EOGLUniform<int>(meshGeometryShader, "metallicTex", 0);
}

void EGeometryPass::RenderMesh(EMeshReference * meshReference)
{
	meshGeometryShader->use();
	Mesh * mesh = meshReference->mesh;
	Asset * asset = meshReference->parent;

	vec3 position = asset->getPosition() + meshReference->positionOffset;
	vec3 scale = asset->getScale() * meshReference->scaleOffset;
	quat rotation = asset->getRotation() * meshReference->rotationOffset;

	if (PBRMaterial * material = (PBRMaterial*)meshReference->getMaterial()) {
		meshUniformModel.Update(glm::scale(glm::translate(mat4(1.0f), position), scale));
		meshUniformRotation.Update(glm::toMat4(rotation));
		meshUniformAO.Update(material->ao);
		meshUniformAlbedo.Update(material->albedo);
		meshUniformMetallic.Update(material->metallic);
		meshUniformRoughness.Update(material->roughness);
		meshUniformRoughnessTex.Update(material->roughnessMap->layer);
		meshUniformAlbedoTex.Update(material->albedoMap->layer);
		meshUniformMetallicTex.Update(material->metallicMap->layer);
		
		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void EGeometryPass::RenderMultiDraw(EMultiDrawContainer * multiCont)
{

}
