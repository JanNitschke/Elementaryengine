#pragma once
#include <EEngine.h>
#include <vector>
#include <EOGLBindlessTexture.h>

using namespace std;
class EOGLFramebuffer
{
public:
	EOGLFramebuffer();
	~EOGLFramebuffer();
	GLuint id;
	vector<EOGLBindlessTexture*> textures;
	void Bind();
	void AppendTexture(EOGLBindlessTexture* texture, GLenum attachment, GLenum type);
	void Complete();
private:
	vector<GLenum> attachments;
};
