#include "EOGLBindlessTexture.h"
#include <iostream>
uint EOGLBindlessTexture::uniformBufferSamplers;

EOGLBindlessTexture::EOGLBindlessTexture(string name)
{
	this->name = name;
	glGenTextures(1, &id);
}

EOGLBindlessTexture::EOGLBindlessTexture(string name, GLenum target, GLenum filter, GLenum internalFormat, GLenum format, GLenum type, int width, int height)
{
	this->name = name;
	glGenTextures(1, &id);
	GenerateTexture(target, filter, internalFormat, format, type, width, height);
	GenerateHandle();
}

EOGLBindlessTexture::~EOGLBindlessTexture()
{

}

void EOGLBindlessTexture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void EOGLBindlessTexture::MakeHandleResident()
{
	glMakeTextureHandleResidentARB(handle);
}

void EOGLBindlessTexture::MakeHandleNonresident()
{
	glMakeTextureHandleNonResidentARB(handle);
}

void EOGLBindlessTexture::GenerateTexture(GLenum target, GLenum filter, GLenum internalFormat, GLenum format, GLenum type, int width, int height)
{
	Bind();
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, NULL);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
}

void EOGLBindlessTexture::SetParameter(GLenum target, GLenum parameter, GLenum value)
{
	Bind();
	glTexParameteri(target, parameter, value);
}

void EOGLBindlessTexture::GenerateHandle()	
{
	handle = glGetTextureHandleARB(id);
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error handle creation " << err << endl;
	}
	MakeHandleResident();
}

void EOGLBindlessTexture::BindAs(Shader * shader, string name)
{
}

void EOGLBindlessTexture::CreateUBO(vector<EOGLBindlessTexture*> textures, int buffer)
{
	vector<GLuint64> texHandles;
	for (auto tex : textures) {
		texHandles.push_back(tex->handle);
	}
	// copy to ubo
	glGenBuffers(1, &uniformBufferSamplers);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferSamplers);
	GLsizeiptr ubs = texHandles.size() * sizeof(GLuint64);
	glBufferData(GL_UNIFORM_BUFFER, ubs, texHandles.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uniformBufferSamplers);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

