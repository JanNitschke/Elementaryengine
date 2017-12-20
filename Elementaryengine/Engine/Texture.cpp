#include "Texture.h"
#include <Lamp.h>
#include <stb_image.h>
#include <stb_image_resize.h>
#include <Game.h>
#include <iostream>
Texture::Texture(char path)
{
	const char* filename = &path;
	Load(filename);
}

Texture::Texture(const char* path, bool toArray)
{
	loadLayer(path);
	
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

void Texture::loadLayer(const char * path)
{
	if (!Game::isServer) {
		int width, height, nrChannels;
		int size = Game::Instance().TextureSize;
		unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
		//unsigned char *fdata;
		//stbir_resize_uint8(data, width, height, 0, fdata, size, size, 0, nrChannels);
		if (width != size || height != size) {
			cout << "Invalid Texture size\n";
			return;
		}
		layer = 10000;
		for each (int i in Game::freeLayers)
		{
			if (i < layer) {
				layer = i;
			}
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY, Game::textureArray);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, size, size,1, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		Game::freeLayers.erase(std::remove(Game::freeLayers.begin(), Game::freeLayers.end(), layer), Game::freeLayers.end());

		stbi_image_free(data);
	}
}

void Texture::Load(const char * path)
{
	if (!Game::isServer) {
		if (path == "") {
			layer = 0;
		}
		else {
			int width, height, nrChannels;
			unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
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
