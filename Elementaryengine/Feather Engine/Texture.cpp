#include "Texture.h"
#include <Lamp.h>
#include <stb_image.h>
#include <stb_image_resize.h>
#include <Game.h>
Texture::Texture(char path)
{
	const char* filename = &path;
	Load(filename);
}

Texture::Texture(char path, bool toArray)
{
	const char* filename = &path;

	if (!Game::isServer && toArray) {
		int width, height, nrChannels;
		unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
		layer = min_element(Game::Instance().freeLayers.begin, Game::Instance().freeLayers.end);
		glBindTexture(GL_TEXTURE_2D, Game::Instance().textureArray);
		glTexSubImage2D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, Game::Instance().TextureSize, Game::Instance().TextureSize, GL_RGB, GL_UNSIGNED_BYTE,data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		handle = glGetTextureHandleARB(id);
		glGetTextureSamplerHandleARB(handle, GL_SAMPLER_2D);
	}
}

Texture::Texture()
{

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_DEPTH_COMPONENT,
		Lamp::SHADOW_WIDTH, Lamp::SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

Texture::Texture(int type)
{
	if (type == 1) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGB,
			Asset::ENVIRONMENT_WIDTH, Asset::ENVIRONMENT_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
}

Texture::Texture(const char * path)
{
	Load(path);

}

void Texture::Load(const char * path)
{
	if (!Game::isServer) {
		int width, height, nrChannels;
		unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		handle = glGetTextureHandleARB(id);
		glGetTextureSamplerHandleARB(handle,GL_SAMPLER_2D);
	}
}

Texture::~Texture()
{
}

unsigned char* Texture::LoadPicture(const char * path)
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	return data;
}
