#include "EShadowPass.h"
#include <Game.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
EShadowPass::EShadowPass()
{
}

EShadowPass::EShadowPass(GLuint vao, GLuint elementBuffer, GLuint indirectBuffer, int meshCount, OUT GLuint* shadowMaps)
{
	VAO = vao;
	ElementBuffer = elementBuffer;
	IndirectBuffer = indirectBuffer;
	this->meshCount = meshCount;
	shadowMaps = &ShadowMaps;
}


EShadowPass::~EShadowPass()
{
}

void EShadowPass::Render()
{
	ERenderPass::Render();
	meshCount = Game::eOpenGl->instance;
	// the number of lights in the Game
	int lightcount = Game::lamps.size();

	// bind the cubemap array containing the shadowmap
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, ShadowMaps);

	// setup the texture array to resize to fit all the lights in the scene
	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT32, shadowMapWidth, shadowMapHeight, 6 * lightcount, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// change the viewport to the resolution of the shadowmaps
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);

	// bind the Texture array to framebuffer and clear it
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ShadowMaps, 0);
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);

	// render each lamp to a layer
	int currentLayer = 0;
	for each (Lamp* l in Game::lamps)
	{

		// only rerender the layer if a shadowmap is needed
		if (l->throwShadows) {

			// create the projection matrix

			// aspect ratio of the shadowMaps
			float aspect = (float)shadowMapWidth / (float)shadowMapHeight;

			// projection matrix fot the shadow map
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, farPlane);
			vec3 lightPos = l->parents[0]->position;

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

			unifromCurrentLayer.Update(currentLayer);

			// copy the created shadow matrix to the GPU
			if (shadowUniformShadowMatrices < 0) {
				shadowUniformShadowMatrices = glGetUniformLocation(_shader->ID, "shadowMatrices");
			}
			glUniformMatrix4fv(shadowUniformShadowMatrices, shadowTransforms.size(), GL_FALSE, glm::value_ptr(shadowTransforms[0]));
			currentLayer++;
			unifromCurrentLightPosition.Update(lightPos);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IndirectBuffer);

			glMultiDrawElementsIndirect(GL_TRIANGLES,
				GL_UNSIGNED_INT,
				(GLvoid*)0,
				meshCount,
				0);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	Game::eOpenGl->shadowMaps = ShadowMaps;
}

void EShadowPass::Initialize()
{
	_shader = Mesh::lightmapShader;

	// Shadow uniforms
	unifromCurrentLayer = EOGLUniform<int>(_shader, "layer", 0);
	unifromCurrentLightPosition = EOGLUniform<vec3>(_shader, "lightPos", vec3(0));

	_uniforms.push_back(new EOGLUniform<int>(_shader, "gPosition", 0));
	_uniforms.push_back(new EOGLUniform<float>(_shader, "far_plane", farPlane));

	ERenderPass::Initialize();

	glGenTextures(1, &ShadowMaps);
	shadowMapWidth = Lamp::SHADOW_WIDTH;
	shadowMapHeight = Lamp::SHADOW_HEIGHT;



}
