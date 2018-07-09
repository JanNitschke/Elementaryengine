#pragma once
#include <gl\glew.h>
#include <GLFW\glfw3.h>
#include <EOpenGl.h>
#include <string>
#include <cstdint>

using namespace std;
class EOGLBindlessTexture
{
public:
	EOGLBindlessTexture(string name);
	EOGLBindlessTexture(string name, GLenum target, GLenum filter, GLenum internalFormat, GLenum format, GLenum type, int width, int height);
	~EOGLBindlessTexture();
	GLuint id;
	string name;
	GLuint64 handle;
	static uint uniformBufferSamplers;

	int locationCache;
	void Bind();
	void MakeHandleResident();
	void MakeHandleNonresident();
	void GenerateTexture(GLenum target, GLenum filter, GLenum internalFormat, GLenum format, GLenum type, int width, int height);
	void SetParameter(GLenum target, GLenum parameter, GLenum value);
	void GenerateHandle();
	void BindAs(Shader * shader, string name);
	static void CreateUBO(vector<EOGLBindlessTexture*> textures,int buffer);
};

