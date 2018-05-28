#include "ERenderPass.h"
#include "Game.h"


ERenderPass::ERenderPass()
{
	// Create Framebuffer
	glGenFramebuffers(1, &renderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderBuffer);
}


ERenderPass::~ERenderPass()
{
	// delete all uniforms
	for each ( EOGLBaseUniform* uniform in _uniforms)
	{
		delete uniform;
	}
}

void ERenderPass::Render()
{
	// activate the correct shader and update all its uniforms
	_shader->use();
	for each (EOGLBaseUniform* uniform in _uniforms)
	{
		uniform->Update();
	}
}

void ERenderPass::Initialize()
{
	// copy the games display settings
	displaySettings = Game::displaySettings;
}
