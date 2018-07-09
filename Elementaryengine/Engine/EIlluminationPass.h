#pragma once
#include "ERenderPass.h"
#include <EOGLFramebuffer.h>

class EIlluminationPass :
	public ERenderPass
{
public:
	EIlluminationPass(EOGLFramebuffer * inBuffer);
	~EIlluminationPass();

	EOGLFramebuffer* inBuffer;
	EOGLFramebuffer* outBuffer;

	virtual void Render();
	virtual void Initialize();


	GLuint lightColorSSBO;
	GLuint lightPositionSSBO;
	GLuint meshDataSSBO;
	GLuint uiElementsSSBO;
	GLuint drawIdOffsetBuffer;

private:
	void EIlluminationPass::SetupLamps(EOpenGl * eOpenGl, Shader * shader);

};

