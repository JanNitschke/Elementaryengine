#include "EOGLFramebuffer.h"
#include <EOpenGL.h>
#include <iostream>

EOGLFramebuffer::EOGLFramebuffer()
{
	glGenFramebuffers(1, &id);
}


EOGLFramebuffer::~EOGLFramebuffer()
{
}

void EOGLFramebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void EOGLFramebuffer::AppendTexture(EOGLBindlessTexture * texture, GLenum attachment, GLenum type)
{
	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, type, texture->id, 0);
	if (attachment != GL_DEPTH_ATTACHMENT) attachments.push_back(attachment);
	textures.push_back(texture);
}

void EOGLFramebuffer::Complete()
{
	GLenum err = 0;
	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error before framebuffer 2: " << err << endl;
	}
	glDrawBuffers(attachments.size(), attachments.data());

	while ((err = glGetError()) != GL_NO_ERROR) {
		cout << "OpenGL error framebuffer: " << err << endl;
	}
	GLenum status = 0;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		std::cout << "Framebuffer not complete!" << status << std::endl;
	}else{
		std::cout << "Framebuffer complete!" << status << std::endl; 
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
