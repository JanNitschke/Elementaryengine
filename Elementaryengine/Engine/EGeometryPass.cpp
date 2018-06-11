#include "EGeometryPass.h"
#include <EOpenGL.h>
#include <Game.h>
EGeometryPass::EGeometryPass()
{
}

EGeometryPass::EGeometryPass(OUT GLuint * positionBuffer, OUT GLuint * normalBuffer, OUT GLuint * albedoSpecBuffer, OUT GLuint * materialBuffer, OUT GLuint * deepthBuffer)
{
	_shader = Mesh::geometryShader;
	_uniforms.push_back(new EOGLUniform<int>(Mesh::geometryShader, "textures", 0));
	_uniforms.push_back(new EOGLUniform<mat4>(Mesh::geometryShader, "VP", []() { return Game::Projection * Game::View; }));
	ERenderPass::Initialize();

	//Setup framebuffer
	glGenFramebuffers(1, &renderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
	EDisplaySettings* settings = Game::displaySettings;


	// - position color buffer
	glGenTextures(1, &PositionBuffer);
	glBindTexture(GL_TEXTURE_2D, PositionBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, settings->windowWidth, settings->windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PositionBuffer, 0);

	// - normal color buffer
	glGenTextures(1, &NormalBuffer);
	glBindTexture(GL_TEXTURE_2D, NormalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, settings->windowWidth, settings->windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalBuffer, 0);

	// - color + specular color buffer
	glGenTextures(1, &AlbedoSpecBuffer);
	glBindTexture(GL_TEXTURE_2D, AlbedoSpecBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, settings->windowWidth, settings->windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, AlbedoSpecBuffer, 0);

	// - material buffer, R = roughtniss, G = Metallic, B = Ambient
	glGenTextures(1, &MaterialBuffer);
	glBindTexture(GL_TEXTURE_2D, MaterialBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, settings->windowWidth, settings->windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, MaterialBuffer, 0);

	glGenTextures(1, &DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, settings->windowWidth, settings->windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuffer, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	*positionBuffer = PositionBuffer;
	*normalBuffer = NormalBuffer;
	*albedoSpecBuffer = AlbedoSpecBuffer;
	*materialBuffer = MaterialBuffer;
	*deepthBuffer = DepthBuffer;
}



EGeometryPass::~EGeometryPass()
{
}

void EGeometryPass::Render()
{
	ERenderPass::Render();

	EOpenGl* eOpenGl = Game::eOpenGl;
	// geometry pass
	// bind and setup framebuffer for geometry pass
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
	glDisable(GL_BLEND);
	glClearColor(0.050f, 0.125f, 0.247f, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//set viewport
	glViewport(0, 0, displaySettings->windowWidth, displaySettings->windowHeight);


	// bind the composed mesh and its buffers
	glBindVertexArray(eOpenGl->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eOpenGl->gElementBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, eOpenGl->gIndirectBuffer);

	// bind the array of textures
	glActiveTexture(GL_TEXTURE0);

	// draw all elements to the geometry buffer
	glMultiDrawElementsIndirect(GL_TRIANGLES,
		GL_UNSIGNED_INT,
		(GLvoid*)0,
		eOpenGl->instance,
		0);

}

void EGeometryPass::Initialize()
{
	_shader = Mesh::geometryShader;
	_uniforms.push_back(new EOGLUniform<int>(Mesh::geometryShader, "textures", 0));
	_uniforms.push_back(new EOGLUniform<mat4>(Mesh::geometryShader, "VP", []() { return Game::Projection * Game::View; }));
	ERenderPass::Initialize();

}
