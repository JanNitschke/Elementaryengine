#pragma once
#include "ERenderPass.h"
#include <EMultiDrawContainer.h>
#include <EMeshReference.h>

#define OUT

class EGeometryPass :
	public ERenderPass
{
public:
	EGeometryPass();
	EGeometryPass(OUT GLuint* positionBuffer, OUT GLuint* normalBuffer, OUT GLuint* albedoSpecBuffer, OUT GLuint* materialBuffer, OUT GLuint* deepthBuffer);
	~EGeometryPass();

	static Shader * meshGeometryShader;

	virtual void Render();
	virtual void Initialize();


	GLuint PositionBuffer;
	GLuint NormalBuffer;
	GLuint AlbedoSpecBuffer;
	GLuint MaterialBuffer;
	GLuint DepthBuffer;
private:
	void RenderMesh(EMeshReference *);
	void RenderMultiDraw(EMultiDrawContainer *);

	EOGLUniform<mat4> meshUniformVP;
	EOGLUniform<mat4> meshUniformModel;
	EOGLUniform<mat4> meshUniformRotation;
	EOGLUniform<vec3> meshUniformAO;
	EOGLUniform<vec3> meshUniformAlbedo;
	EOGLUniform<float> meshUniformRoughness;
	EOGLUniform<float> meshUniformMetallic;
	EOGLUniform<int> meshUniformRoughnessTex;
	EOGLUniform<int> meshUniformAlbedoTex;
	EOGLUniform<int> meshUniformMetallicTex;
	EOGLUniform<int> meshUniformTextureArray;

};

