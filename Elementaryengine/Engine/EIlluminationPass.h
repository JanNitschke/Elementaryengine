#pragma once
#include "ERenderPass.h"
class EIlluminationPass :
	public ERenderPass
{
public:
	EIlluminationPass();
	EIlluminationPass(GLuint positionBuffer, GLuint normalBuffer, GLuint albedoSpecBuffer, GLuint materialBuffer, GLuint depthBuffer);
	~EIlluminationPass();

	virtual void Render();
	virtual void Initialize();

	GLuint PositionBuffer;
	GLuint NormalBuffer;
	GLuint AlbedoSpecBuffer;
	GLuint MaterialBuffer;
	GLuint DepthBuffer;

	GLuint lightColorSSBO;
	GLuint lightPositionSSBO;
	GLuint meshDataSSBO;
	GLuint uiElementsSSBO;
	GLuint drawIdOffsetBuffer;

	GLuint frameOut;

private:
	void EIlluminationPass::SetupLamps(EOpenGl * eOpenGl, Shader * shader);

};

