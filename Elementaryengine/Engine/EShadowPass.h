#pragma once
#include "ERenderPass.h"
#define OUT

class EShadowPass :
	public ERenderPass
{
public:
	EShadowPass();
	EShadowPass(GLuint vao,GLuint elementBuffer,GLuint indirectBuffer, int meshCount,OUT GLuint* shadowMaps);

	~EShadowPass();


	virtual void Render();
	virtual void Initialize();

	GLuint ShadowMaps;


	GLuint VAO;
	GLuint ElementBuffer;
	GLuint IndirectBuffer;
private:
	uint shadowMapWidth;
	uint shadowMapHeight;
	EOGLUniform<int> unifromCurrentLayer;
	EOGLUniform<vec3> unifromCurrentLightPosition;
	float nearPlane = 1.0f;
	float farPlane = 25.0f;

	int meshCount;
	int shadowUniformShadowMatrices = -1;
};

