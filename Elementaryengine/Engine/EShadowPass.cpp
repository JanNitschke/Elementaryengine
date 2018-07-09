#include "EShadowPass.h"
#include <Game.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <EModularRasterizer.h>

Shader* EShadowPass::meshShadowShader;


EShadowPass::EShadowPass()
{
	Initialize();
	shadowMaps = new EOGLBindlessTexture("shadowMaps");
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMaps->id);

	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT32, shadowMapWidth, shadowMapHeight, 6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);


	shadowMaps->GenerateHandle();
	framebuffer = new EOGLFramebuffer();
	framebuffer->Bind();
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMaps->id, 0);
}


EShadowPass::~EShadowPass()
{

}

void EShadowPass::Render()
{
	GLenum err = 0;
	
	framebuffer->Bind();
	ERenderPass::Render();
	meshCount = Game::eOpenGl->instance;

	// the number of lights in the Game
	int lightcount = Game::lamps.size();

	// bind the cubemap array containing the shadowmap
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMaps->id);

	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error bind tex : " << err << endl;
	}
	// setup the texture array to resize to fit all the lights in the scene
	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT32, shadowMapWidth, shadowMapHeight, 6 * lightcount, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	while ((err = glGetError()) != GL_NO_ERROR) {
		// clear errors 
		// throws error if texture size did not change
	}

	// change the viewport to the resolution of the shadowmaps
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);

	// bind the Texture array to framebuffer and clear it
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);

	// render each lamp to a layer
	int currentLayer = 0;
	for each (Lamp* l in Game::lamps)
	{

		// only rerender the layer if a shadowmap is needed
		if (l->throwShadows && l->parent != nullptr){

			// aspect ratio of the shadowMaps
			float aspect = (float)shadowMapWidth / (float)shadowMapHeight;

			// projection matrix fot the shadow map
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
			vec3 lightPos = l->parent->getPosition();

			// transform matricies for the shadow map
			std::vector<glm::mat4> shadowTransforms;
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

			// Update the layer uniform
			unifromCurrentLayer.Update(currentLayer);
			unifromCurrentLightPosition.Update(lightPos);

			_shader->use();

			// copy the created shadow matrix to the GPU for MDI shader and Mesh Shader
			if (shadowUniformShadowMatrices < 0) {
				shadowUniformShadowMatrices = glGetUniformLocation(_shader->ID, "shadowMatrices");
			}
			glUniformMatrix4fv(shadowUniformShadowMatrices, shadowTransforms.size(), GL_FALSE, glm::value_ptr(shadowTransforms[0]));

			meshShadowShader->use();
			if (shadowUniformShadowMeshMatrices < 0) {
				shadowUniformShadowMeshMatrices = glGetUniformLocation(meshShadowShader->ID, "shadowMatrices");
			}
			glUniformMatrix4fv(shadowUniformShadowMeshMatrices, shadowTransforms.size(), GL_FALSE, glm::value_ptr(shadowTransforms[0]));

			meshUnifromCurrentLayer.Update(currentLayer);
			meshUnifromCurrentLightPosition.Update(lightPos);
			meshUnifromFar.Update();
			meshUniformVP.Update();
			currentLayer++;


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
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void EShadowPass::Initialize()
{
	_shader = Mesh::lightmapShader;
	meshShadowShader = new Shader("..\\shaders\\GeometryPassMesh.vert", "..\\shaders\\GeometryPassLight.geom", "..\\shaders\\GeometryPassLight.frag");
	// Shadow uniforms
	unifromCurrentLayer = EOGLUniform<int>(_shader, "layer", 0);
	unifromCurrentLightPosition = EOGLUniform<vec3>(_shader, "lightPos", vec3(0));

	_uniforms.push_back(new EOGLUniform<int>(_shader, "gPosition", 0));
	_uniforms.push_back(new EOGLUniform<float>(_shader, "far_plane", farPlane));

	meshUniformVP = EOGLUniform<mat4>(meshShadowShader, "VP", []() { return Game::Projection * Game::View; });
	meshUniformModel = EOGLUniform<mat4>(meshShadowShader, "Model", mat4(0));
	meshUniformRotation = EOGLUniform<mat4>(meshShadowShader, "Rot", mat4(0));
	meshUnifromCurrentLayer = EOGLUniform<int>(meshShadowShader, "layer", 0);
	meshUnifromCurrentLightPosition = EOGLUniform<vec3>(meshShadowShader, "lightPos", vec3(0));
	meshUnifromFar = EOGLUniform<float>(meshShadowShader, "far_plane", 25.0f);
	ERenderPass::Initialize();

	shadowMapWidth = Lamp::SHADOW_WIDTH;
	shadowMapHeight = Lamp::SHADOW_HEIGHT;
	
}

void EShadowPass::RenderMesh(EMeshReference * meshReference)
{
	meshShadowShader->use();
	Mesh * mesh = meshReference->mesh;
	Asset * asset = meshReference->parent;

	vec3 position = asset->getPosition() + meshReference->positionOffset;
	vec3 scale = asset->getScale() * meshReference->scaleOffset;
	quat rotation = asset->getRotation() * meshReference->rotationOffset;
	if (PBRMaterial * material = (PBRMaterial*)meshReference->getMaterial()) {

		meshUniformModel.Update(glm::scale(glm::translate(mat4(1.0f), position), scale));
		meshUniformRotation.Update(glm::toMat4(rotation));

		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void EShadowPass::RenderMultiDraw(EMultiDrawContainer * multiCont)
{
	/*
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IndirectBuffer);

	glMultiDrawElementsIndirect(GL_TRIANGLES,
		GL_UNSIGNED_INT,
		(GLvoid*)0,
		meshCount,
		0);
		*/
}
