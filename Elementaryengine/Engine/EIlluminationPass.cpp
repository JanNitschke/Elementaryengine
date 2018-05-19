#include "EIlluminationPass.h"
#include <Game.h>



EIlluminationPass::EIlluminationPass()
{

}

EIlluminationPass::EIlluminationPass(GLuint positionBuffer, GLuint normalBuffer, GLuint albedoSpecBuffer, GLuint materialBuffer, GLuint depthBuffer)
{
	PositionBuffer = positionBuffer;
	NormalBuffer = normalBuffer;
	AlbedoSpecBuffer = albedoSpecBuffer;
	MaterialBuffer = materialBuffer;
	DepthBuffer = depthBuffer;
	glGenTextures(1, &frameOut);

}


EIlluminationPass::~EIlluminationPass()
{
}



void EIlluminationPass::Render()
{
	ERenderPass::Render();
	// bind and setup framebuffer for lighting pass
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
	glBindTexture(GL_TEXTURE_2D, frameOut);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, displaySettings->windowWidth, displaySettings->windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameOut, 0);


	// clear buffers
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind Position buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PositionBuffer);
	// Value 0

	// bind Normal buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalBuffer);

	// bind AlbedoSpec buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, AlbedoSpecBuffer);

	// bind MaterialBuffer texture and set its uniform
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, MaterialBuffer);


	// bind depth texture and set its uniform
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);


	SetupLamps(Game::eOpenGl, _shader);

	Game::eOpenGl->renderQuad();
}

void EIlluminationPass::Initialize()
{
	_shader = Mesh::pbrShader;
	_uniforms.push_back(new EOGLUniform<vec3>(_shader, "viewPos", []() {return Game::activeCam->position; }));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gPosition", 0));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gNormal", 1));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gAlbedoSpec", 2));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gMaterial", 3));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gDepth", 6));

	_uniforms.push_back(new EOGLUniform<int>(_shader, "gDepth", 6));
	_uniforms.push_back(new EOGLUniform<mat4>(_shader, "invProj", []() {return inverse(Game::Projection); }));
	_uniforms.push_back(new EOGLUniform<mat4>(_shader, "invView", []() {return inverse(Game::View); }));
	_uniforms.push_back(new EOGLUniform<float>(_shader, "far_plane", 25.0f));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "shadowMaps", 8));

	ERenderPass::Initialize();

	//Setup framebuffer
	glGenFramebuffers(1, &renderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);

	glGenBuffers(1, &lightColorSSBO);
	glGenBuffers(1, &lightPositionSSBO);
	glGenBuffers(1, &meshDataSSBO);
	glGenBuffers(1, &uiElementsSSBO);
	glGenBuffers(1, &drawIdOffsetBuffer);
}

void EIlluminationPass::SetupLamps(EOpenGl * eOpenGl, Shader * shader)
{
	// create vectors for light colors and positions
	vector<vec4> lightColors;
	vector<vec4> lightPositions;

	// add color and position for each light to vectors
	for each (Lamp* l in Game::lamps) {
		vec3 outcol = l->color;
		vec3 outpos = l->parents[0]->position;
		lightColors.push_back(vec4(outcol, 0));
		lightPositions.push_back(vec4(outpos, 0));
	}

	// dont do this for the ssr shader
	if (shader != Mesh::ssrShader) {
		// bind Shadowmap texture array and set its uniform
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, Game::eOpenGl->shadowMaps);
	}


	// copy color SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
	GLsizeiptr lcs = sizeof(glm::vec4) * (lightColors.size());
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, lightColorSSBO, 0, lcs);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lcs, lightColors.data(), GL_DYNAMIC_DRAW);

	// copy position SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightPositionSSBO);
	GLsizeiptr lps = sizeof(glm::vec4) * lightPositions.size();
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4, lightPositionSSBO, 0, lps);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lps, lightPositions.data(), GL_DYNAMIC_DRAW);
}