#pragma once
#include "ERenderPass.h"
#define OUT
class EGeometryPass :
	public ERenderPass
{
public:
	EGeometryPass();
	EGeometryPass(OUT GLuint* positionBuffer, OUT GLuint* normalBuffer, OUT GLuint* albedoSpecBuffer, OUT GLuint* materialBuffer, OUT GLuint* deepthBuffer);
	~EGeometryPass();

	virtual void Render();
	virtual void Initialize();

	GLuint PositionBuffer;
	GLuint NormalBuffer;
	GLuint AlbedoSpecBuffer;
	GLuint MaterialBuffer;
	GLuint DepthBuffer;
};

