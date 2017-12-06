#include "stdafx.h"
#include "BallDemoMode.h"
#include <Feather.h>
#include <glm\glm.hpp>
#include <Model.h>
#include <Mesh.h>
#include <Lamp.h>
#include <FPCam.h>
#include <Terrain.h>

using namespace glm;
using namespace std;

double timerunning = 0;

BallDemoMode::BallDemoMode()
{
}

BallDemoMode::~BallDemoMode()
{
}

void BallDemoMode::Tick(double deltaTime)
{
}

void BallDemoMode::Load()
{
	game->SetActiveCam(new FPCam());
	Model* m = new Model("Assets/Meshs/Cube.obj");
	Model* m2 = new Model("Assets/Meshs/Sphere.obj");
	Model* mpbr = new Model("Assets/Meshs/Sphere.obj");

	Model* mpbr2 = new Model("Assets/Meshs/Sphere.obj");
	Model* ter = new Model("Assets/Meshs/terrain512.obj");

	Asset* a = new Asset(vec3(0,-3,0),vec3(5,0.1f,5),0,assetShapes::cube);
	a->setFriction(2);
	m->attachTo(a);
	a->OnTick = FallingTick;
	a->renderEnvironment = false;
	//DefaultMaterial* wood = new DefaultMaterial();
	//wood->ambient = vec3(0.01f, 0.01f, 0.01f);
	//wood->diffuse = vec3(0.6f, 0.6f, 0.6f);
	//wood->specular = vec3(0.1f);
	//wood->shininess = 32;
	//wood->diffuseMap = new Texture("Assets/Textures/Wood.jpg");
	//wood->specularMap = new Texture("Assets/Textures/WoodSpec.jpg");
	//wood->TextureScale = vec2(3.0f);

	PBRMaterial* pbrwood = new PBRMaterial();
	pbrwood->albedo = vec3(100);
	pbrwood->albedoMap = new Texture("Assets/Textures/Wood.jpg");
	pbrwood->ao = vec3(0.01f);
	pbrwood->metallic = 0;
	pbrwood->roughness = 0.4f;
	pbrwood->roughnessMap = new Texture("Assets/Textures/WoodSpec.jpg");
	pbrwood->TextureScale = vec2(3.0f);

	m->meshes[0]->material = pbrwood;

	//DefaultMaterial* beton = new DefaultMaterial();
	//beton->ambient = vec3(0.5, 0.5, 0.5);
	//beton->diffuse = vec3(0.6f);
	//beton->specular = vec3(0.8f);
	//beton->shininess = 64;
	//beton->diffuseMap = new Texture("Assets/Textures/Plaster.jpg");
	//beton->specularMap = new Texture("Assets/Textures/PlasterSpec.jpg");

	PBRMaterial* beton = new PBRMaterial();
	beton->albedo = vec3(100);
	beton->albedoMap = new Texture("Assets/Textures/Plaster.jpg");
	beton->ao = vec3(0.01f);
	beton->metallic = 0;
	beton->roughness = 0.4f;
	beton->roughnessMap = new Texture("Assets/Textures/PlasterR.jpg");

	PBRMaterial* grass = new PBRMaterial();
	grass->albedo = vec3(100);
	grass->albedoMap = new Texture("Assets/Textures/RockGrass.jpg");
	grass->ao = vec3(0.01f);
	grass->metallic = 0;
	grass->roughness = 10.0f;
	grass->TextureScale = vec2(50);
	grass->roughnessMap = new Texture("Assets/Textures/RockSpec.jpg");

	m2->meshes[0]->material = beton;

	PBRMaterial* rust = new PBRMaterial();
	rust->albedo = vec3(20);
	rust->albedoMap = new Texture("Assets/Textures/r2albedo.jpg");
	rust->ao = vec3(0.5);
	rust->metallic = 1;
	rust->metallicMap = new Texture("Assets/Textures/r2metal.jpg");
	rust->roughness = 0.2f;
	rust->roughnessMap = new Texture("Assets/Textures/r2rough.jpg");

	mpbr->meshes[0]->material = rust;
	mpbr2->meshes[0]->material = rust;


	Model* m3 = new Model("Assets/Meshs/Sphere.obj");
	DefaultMaterial* lampmat = new DefaultMaterial();
	lampmat->ambient = vec3(1000.0f);
	lampmat->diffuse = vec3(1.0f);
	lampmat->specular = vec3(1.0f);
	lampmat->shininess = 8;

	m3->meshes[0]->material = lampmat;

	Lamp* l = new Lamp();
	Asset* b = new Asset();
	l->attachTo(b);
	l->color = vec3(3.0);
	b->scale = vec3(.10f);
	b->position = vec3(1.0f, 1.0f, 1.0f);
	b->OnTick = LampTick;
	m3->attachTo(b);


	Lamp* l2 = new Lamp();
	Asset* b2 = new Asset();
	l2->attachTo(b2);
	l2->color = vec3(0.8f);
	b2->scale = vec3(.10f);
	b2->position = vec3(2.5f, 6, 1.0f);
	m3->attachTo(b2);
	
	Asset* c = new Asset(vec3(0.5f, 20, 0),vec3(.4f),100,assetShapes::ball);
	m2->attachTo(c);
	c->renderEnvironment = false;

	Asset* d = new Asset(vec3(2,-0.5,2),vec3(.40f),10,assetShapes::ball);
	mpbr->attachTo(d);
	d->renderEnvironment = true;

	Asset* e = new Asset(vec3(0.1f, -0.5, 0), vec3(.4f), 10, assetShapes::ball);
	mpbr2->attachTo(e);
	e->renderEnvironment = true;

	//Asset* t = new Asset(vec3(0, -10, 0), vec3(50, 10, 50), 0, assetShapes::cube);
	//Mesh* mesh = ter->meshes[0];
	//mesh->material = beton;
	//Texture* tt = new Texture("Assets/Textures/stoneHeight.jpg");
	//t->setHeightmapCollision("Assets/Textures/stoneHeight.jpg");
	//Terrain* terrain = new Terrain(mesh,tt);
	//terrain->material = grass;
	//terrain->grassMap = new Texture("Assets/Textures/RockSpec.jpg"); 
	//terrain->attachTo(t);
}

void BallDemoMode::Start()
{
}

void BallDemoMode::Stop()
{
}

void LampTick(GLFWwindow * window, double deltaTime, Asset * asset)
{
	timerunning += deltaTime;

	float radius = 10.0f;
	float lz = (float)sin(timerunning * 0.5f) * radius;
	float lx = (float)cos(timerunning *  0.5f) * radius;
	asset->setPosition(vec3(lx, 1.50f, lz));
}

void FallingTick(GLFWwindow * window, double deltaTime, Asset * asset)
{
	asset->setPosition(asset->position + vec3(0, 1, 0));
}
