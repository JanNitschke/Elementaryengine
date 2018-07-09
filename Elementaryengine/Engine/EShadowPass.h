#pragma once
#include "ERenderPass.h"
#include <EMultiDrawContainer.h>
#include <EMeshReference.h>
#include <EOGLBindlessTexture.h>
#include <EOGLFramebuffer.h>


class EShadowPass :
	public ERenderPass
{
public:
	EShadowPass();

	~EShadowPass();

	static Shader * meshShadowShader;

	virtual void Render();
	virtual void Initialize();
	void RenderMesh(EMeshReference * meshReference);
	void RenderMultiDraw(EMultiDrawContainer * multiCont);
	EOGLBindlessTexture * shadowMaps;
	EOGLFramebuffer * framebuffer;
	GLuint VAO;
	GLuint ElementBuffer;
	GLuint IndirectBuffer;
private:
	uint shadowMapWidth;
	uint shadowMapHeight;

	float nearPlane = 1.0f;
	float farPlane = 25.0f;

	EOGLUniform<int> unifromCurrentLayer;
	EOGLUniform<vec3> unifromCurrentLightPosition;

	EOGLUniform<int> meshUnifromCurrentLayer;
	EOGLUniform<float> meshUnifromFar;
	EOGLUniform<vec3> meshUnifromCurrentLightPosition;
	EOGLUniform<mat4> meshUniformVP;
	EOGLUniform<mat4> meshUniformModel;
	EOGLUniform<mat4> meshUniformRotation;
	int meshCount;
	int shadowUniformShadowMatrices = -1;
	int	shadowUniformShadowMeshMatrices = -1;
};

