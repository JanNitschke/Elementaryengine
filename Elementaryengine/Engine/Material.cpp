#include "Material.h"

Material::Material()
{
	TextureScale = vec2(1.0f);
}


Material::~Material()
{
}

void Material::SetMat()
{
}

PBRMaterial::PBRMaterial()
{
	albedoMap = new Texture();
	roughnessMap = new Texture();
	metallicMap = new Texture();
	normalMap = new Texture();
	aoMap = new Texture();
	albedo = vec3(1);
	roughness = 0.5;
	metallic = 0;
	ao = vec3(1);
}

PBRMaterial::~PBRMaterial()
{
}

void PBRMaterial::SetMat()
{
}

DefaultMaterial::DefaultMaterial()
{
	ambientMap = new Texture();
	diffuseMap = new Texture();
	specularMap = new Texture();
	normalMap = new Texture();
	ambient = vec3(0);
	specular = vec3(1);
	diffuse = vec3(1);
	shininess = 32;
}

DefaultMaterial::~DefaultMaterial()
{
}

void DefaultMaterial::SetMat()
{
}
