#pragma once
#include "ERenderPass.h"
#include <EOGLFramebuffer.h>

class EPostPass :
	public ERenderPass
{
public:
	EPostPass(GLuint positionBuffer, GLuint normalBuffer, GLuint albedoSpecBuffer, GLuint materialBuffer, GLuint colorBuffer, GLuint depthBuffer);
	EPostPass(EOGLFramebuffer * geometrybuffer, EOGLFramebuffer * colorbuffer);

	~EPostPass();

	EOGLFramebuffer * geometrybuffer;
	EOGLFramebuffer * colorbuffer;

	virtual void Render();
	virtual void Initialize();

	GLuint uiElementsSSBO;

private:
	void BuildUI();
	vector<ERendererUIElement> ERUIElements;

};

