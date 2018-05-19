#pragma once
#include "ERenderPass.h"
class EPostPass :
	public ERenderPass
{
public:
	EPostPass(GLuint positionBuffer, GLuint normalBuffer, GLuint albedoSpecBuffer, GLuint materialBuffer, GLuint colorBuffer, GLuint depthBuffer);
	EPostPass();

	~EPostPass();

	virtual void Render();
	virtual void Initialize();

	GLuint PositionBuffer;
	GLuint NormalBuffer;
	GLuint AlbedoSpecBuffer;
	GLuint MaterialBuffer;
	GLuint ColorBuffer;
	GLuint DepthBuffer;

	GLuint uiElementsSSBO;

private:
	void BuildUI();
	vector<ERendererUIElement> ERUIElements;

};

