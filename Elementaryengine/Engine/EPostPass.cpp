#include "EPostPass.h"
#include <Game.h>



EPostPass::EPostPass(GLuint positionBuffer, GLuint normalBuffer, GLuint albedoSpecBuffer, GLuint materialBuffer, GLuint colorBuffer, GLuint depthBuffer)
{
	PositionBuffer = positionBuffer;
	NormalBuffer = normalBuffer;
	AlbedoSpecBuffer = albedoSpecBuffer;
	MaterialBuffer = materialBuffer;
	ColorBuffer = colorBuffer;
	DepthBuffer = depthBuffer;

}

EPostPass::EPostPass()
{

}


EPostPass::~EPostPass()
{
}

void EPostPass::Render()
{
	ERenderPass::Render();
	//post fx (ssr) 
	glBlitFramebuffer(0, 0, displaySettings->windowWidth, displaySettings->windowHeight, 0, 0, displaySettings->windowWidth, displaySettings->windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// clear the frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind Position buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PositionBuffer);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalBuffer);


	// bind AlbedoSpec buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, AlbedoSpecBuffer);


	// bind MaterialBuffer texture and set its uniform
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, MaterialBuffer);


	// bind Lighting pass output texture and set its uniform
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);


	// bind depth texture and set its uniform
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, DepthBuffer);


	BuildUI();

	Game::eOpenGl->renderQuad();


	// copy content of geometry's depth buffer to default framebuffer's depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, DepthBuffer);

}

void EPostPass::Initialize()
{
	_shader = Mesh::ssrShader;

	_uniforms.push_back(new EOGLUniform<int>(_shader, "gPosition", 0));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gNormal", 1));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gAlbedoSpec", 2));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gMaterial", 3));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gDepth", 6));
	//_uniforms.push_back(new EOGLUniform<float>(_shader, "far_plane", 25.0f));
	_uniforms.push_back(new EOGLUniform<int>(_shader, "gColor", 5));
	//_uniforms.push_back(new EOGLUniform<int>(_shader, "textures", 0));

	//_uniforms.push_back(new EOGLUniform<vec3>(_shader, "viewPos",	[](){return Game::activeCam->position; }));
	//_uniforms.push_back(new EOGLUniform<mat4>(_shader, "view",		[](){return Game::View; }));
	//_uniforms.push_back(new EOGLUniform<mat4>(_shader, "invView",	[](){return inverse(Game::View); }));
	//_uniforms.push_back(new EOGLUniform<mat4>(_shader, "proj",		[](){return Game::Projection; }));
	//_uniforms.push_back(new EOGLUniform<mat4>(_shader, "invProj",	[](){return inverse(Game::Projection); }));
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
	// copy the atribute vector to the GPU
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, uiElementsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ERendererUIElement) * ERUIElements.size(), ERUIElements.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}
