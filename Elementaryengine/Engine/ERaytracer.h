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
	GLuint ray00Uniform = -1;
	GLuint ray01Uniform = -1;
	GLuint ray10Uniform = -1;
	GLuint ray11Uniform = -1;
	GLuint eyeUniform = -1;
	GLuint triangleBuffer;

	Shader* rayComputeShader;
	Shader* rayDisplayShader;

	template <typename T>
	T nextPowerOfTwo(T v)
	{
		if (v < 1) return 0;

		--v;
		for (int i = 0; i < sizeof v; ++i)
			v |= v >> (1 << i);
		return v + 1;
	}
};

struct RaytracerTriangle {
	vec4 p0;
	vec4 p1;
	vec4 p2;
	vec4 normal;	
	//vec2 p0T;
	//vec2 p1T;
	//vec2 p2T;
	int drawData;
	float placeholder[3];
};
struct RaytracerTriangleDrawData {
	vec3 albedo;
	float roughness;
};