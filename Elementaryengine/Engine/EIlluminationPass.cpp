#include "EIlluminationPass.h"
#include <Game.h>


EIlluminationPass::EIlluminationPass(EOGLFramebuffer * inBuffer)
{
	ERenderPass::Initialize();
	glGenBuffers(1, &lightColorSSBO);
	glGenBuffers(1, &lightPositionSSBO);
	this->inBuffer = inBuffer;
	outBuffer = new EOGLFramebuffer();
	outBuffer->Bind();
	EOGLBindlessTexture* positionBuffer = new EOGLBindlessTexture("illuminationBuffer", GL_TEXTURE_2D, GL_NEAREST, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, displaySettings->windowWidth, displaySettings->windowHeight);
	outBuffer->AppendTexture(positionBuffer, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);

	outBuffer->Complete();
}



EIlluminationPass::~EIlluminationPass()
{
}



void EIlluminationPass::Render()
{
	GLenum err;

	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error before illu " << err << endl;
	}
	ERenderPass::Render();
	outBuffer->Bind();
	// clear buffers
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error before lamps illu pass " << err << endl;
	}
	SetupLamps(Game::eOpenGl, _shader);
	Game::eOpenGl->renderQuad();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error in illu pass " << err << endl;
	}
}

void EIlluminationPass::Initialize()
{
	_shader = Mesh::pbrShader;
	_shader->use();

	_uniforms.push_back(new EOGLUniform<vec3>(_shader, "viewPos", []() {return Game::activeCam->position; }));
	_uniforms.push_back(new EOGLUniform<mat4>(_shader, "invProj", []() {return inverse(Game::Projection); }));
	_uniforms.push_back(new EOGLUniform<mat4>(_shader, "invView", []() {return inverse(Game::View); }));
	_uniforms.push_back(new EOGLUniform<float>(_shader, "far_plane", 25.0f));
}

void EIlluminationPass::SetupLamps(EOpenGl * eOpenGl, Shader * shader)
{
	// create vectors for light colors and positions
	vector<vec4> lightColors;
	vector<vec4> lightPositions;

	// add color and position for each light to vectors
	for each (Lamp* l in Game::lamps) {
		if (l->throwShadows && l->parent != nullptr) {
			vec3 outcol = l->color;
			vec3 outpos = l->parent->position;
			lightColors.push_back(vec4(outcol, 0));
			lightPositions.push_back(vec4(outpos, 0));
		}

	}
	_shader->use();
	
	// copy color SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightColorSSBO);
	GLsizeiptr lcs = sizeof(glm::vec4) * (lightColors.size());
	glBufferData(GL_SHADER_STORAGE_BUFFER, lcs, lightColors.data(), GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 15, lightColorSSBO, 0, lcs);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, lightColorSSBO);

	// copy position SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightPositionSSBO);
	GLsizeiptr lps = sizeof(glm::vec4) * lightPositions.size();
	glBufferData(GL_SHADER_STORAGE_BUFFER, lps, lightPositions.data(), GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 16, lightPositionSSBO, 0, lps);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 16, lightPositionSSBO);
	
}