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
	albedoMap = new Texture("empty.jpg");
	roughnessMap = new Texture("empty.jpg");
	metallicMap = new Texture("empty.jpg");
	normalMap = new Texture("empty.jpg");
	aoMap = new Texture("empty.jpg");
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
	ambientMap = new Texture("empty.jpg");
	diffuseMap = new Texture("empty.jpg");
	specularMap = new Texture("empty.jpg");
	normalMap = new Texture("empty.jpg");
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
