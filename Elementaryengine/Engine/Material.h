#pragma once
#include <EEngine.h>
#include <Texture.h>
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
class DllExport Material
{
public:
	Material();
	~Material();
	vec2 TextureScale;	
	virtual void SetMat();
};

class DllExport DefaultMaterial:
	public Material
{
public:
	DefaultMaterial();
	~DefaultMaterial();

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	Texture* ambientMap;
	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;
	float shininess;
	virtual void SetMat();
};

class DllExport PBRMaterial:
	public Material
{
public:
	PBRMaterial();
	~PBRMaterial();

	vec3 albedo;
	float roughness;
	float metallic;
	vec3 ao;
	Texture* albedoMap;
	Texture* roughnessMap;
	Texture* metallicMap;
	Texture* normalMap;
	Texture* aoMap;
	virtual void SetMat();

};

