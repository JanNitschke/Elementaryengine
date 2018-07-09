#include "EPostPass.h"
#include <Game.h>
#include <EModularRasterizer.h>



EPostPass::EPostPass(EOGLFramebuffer * geometrybuffer, EOGLFramebuffer * colorbuffer)
{
	this->geometrybuffer = geometrybuffer;
	this->colorbuffer = colorbuffer;
}


EPostPass::~EPostPass()
{
}

void EPostPass::Render()
{
	GLenum err;

	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error before post " << err << endl;
	}
	ERenderPass::Render();
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error before framebuffer post " << err << endl;
	}
	//post fx (ssr) 
	glBlitFramebuffer(0, 0, displaySettings->windowWidth, displaySettings->windowHeight, 0, 0, displaySettings->windowWidth, displaySettings->windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error framebuffer post " << err << endl;
	}
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error after framebuffer post " << err << endl;
	}

	// clear the frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BuildUI();

	Game::eOpenGl->renderQuad();

	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error end post " << err << endl;
	}
}

void EPostPass::Initialize()
{
	_shader = Mesh::ssrShader;
	_shader->use();
	_uniforms.push_back(new EOGLUniform<float>(_shader, "far_plane", 25.0f));
	_uniforms.push_back(new EOGLUniform<vec3>(_shader, "viewPos",	[](){return Game::activeCam->position; }));
	_uniforms.push_back(new EOGLUniform<mat4>(_shader, "view",		[](){return Game::View; }));
	_uniforms.push_back(new EOGLUniform<mat4>(_shader, "invView",	[](){return inverse(Game::View); }));
	_uniforms.push_back(new EOGLUniform<mat4>(_shader, "proj",		[](){return Game::Projection; }));
	_uniforms.push_back(new EOGLUniform<mat4>(_shader, "invProj",	[](){return inverse(Game::Projection); }));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "screenX",	[](){return Game::displaySettings->windowWidth; }));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "screenY",	[](){return Game::displaySettings->windowHeight; }));

	ERenderPass::Initialize();
	glGenBuffers(1, &uiElementsSSBO);
}

void EPostPass::BuildUI()
{
	ERUIElements.clear();
	ERUIElements.resize(0);
	for each(UIElement* uie in Game::uiElements) {
		ERendererUIElement u = ERendererUIElement();
		u.positionPixel = uie->positionPixel;
		u.posisionPercent = uie->posisionPercent;
		u.sizePixel = uie->sizePixel;
		u.sizePercent = uie->sizePercent;

		u.foregroundColor = uie->foregroundColor;
		u.backgroundColor = uie->backgroundColor;
		u.texture = uie->texture->layer;
		u.alphamap = uie->alphamap->layer;
		u.backgoundBlur = uie->backgoundBlur;
		u.foregroundBlur = uie->foregroundBlur;
		u.opacity = uie->opacity;
		u.z = uie->zindex;
		ERUIElements.push_back(u);
	}
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error ui 1: " << err << endl;
	}
	// copy the atribute vector to the GPU
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, uiElementsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ERendererUIElement) * ERUIElements.size(), ERUIElements.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error ui 2: " << err << endl;
	}
}
