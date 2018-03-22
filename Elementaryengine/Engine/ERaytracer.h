#pragma once
#include "ERender.h"
class ERaytracer :
	public ERenderer
{
public:
	ERaytracer();
	~ERaytracer();

	void Setup(EOpenGl* eOpenGl, EDisplaySettings* displaySettings);
	void SetupFrame(bool meshChanged, EOpenGl* eOpenGl);
	void RenderFrame(EOpenGl* eOpenGl, EDisplaySettings* displaySettings, mat4 View, mat4 Projection);
	void RenderFX(EOpenGl* eOpenGl, EDisplaySettings* displaySettings);
	Texture* loadTexture(const char* path);
private:
	GLuint outputTexture;
	GLuint outputTextureUniform = -1;

	Shader* rayComputeShader;
	Shader* rayDisplayShader;

};

