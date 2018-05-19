#include "ERenderPass.h"
#include "Game.h"


ERenderPass::ERenderPass()
{

	//Setup framebuffer
	glGenFramebuffers(1, &renderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
}


ERenderPass::~ERenderPass()
{
	for each ( EOGLBaseUniform* uniform in _uniforms)
	{
		delete uniform;
	}
}

void ERenderPass::Render()
{
	_shader->use();
	for each (EOGLBaseUniform* uniform in _uniforms)
	{
		uniform->Update();
	}
	return;
}

void ERenderPass::Initialize()
{
	displaySettings = Game::displaySettings;
}
