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
	Model* lamp = new Model("Assets/Meshs/Sphere.obj");
	Model* table = new Model("Assets/Meshs/tt.obj");

	Mesh* cube = m->meshes[0];
	Mesh* sphere = lamp->meshes[0];
	Mesh* tab = table->meshes[0];

	Asset* a = new Asset(vec3(0,-3,0),vec3(50,0.1f,50),0,assetShapes::cube);
	a->setFriction(2);
	cube->attachTo(a);
	a->OnTick = FallingTick;
	a->renderEnvironment = false;

	PBRMaterial* pbrwood = new PBRMaterial();
	pbrwood->albedo = vec3(1);
	pbrwood->albedoMap = new Texture("Assets/Textures/Wood.jpg",true);
	pbrwood->ao = vec3(0.01f);
	pbrwood->metallic = 0;
	pbrwood->roughness = 0.4f;
	pbrwood->roughnessMap = new Texture("Assets/Textures/WoodSpec.jpg",true);
	pbrwood->TextureScale = vec2(1.0f);


	PBRMaterial* twood = new PBRMaterial();
	twood->albedo = vec3(1);
	twood->albedoMap = new Texture("Assets/Textures/tt.jpg", true);
	twood->ao = vec3(0.01f);
	twood->metallic = 0;
	twood->roughness = 3.0f;
	twood->roughnessMap = new Texture("Assets/Textures/ttr.jpg", true);
	twood->TextureScale = vec2(1.0f);

	PBRMaterial* floor = new PBRMaterial();
	floor->albedo = vec3(1);
	floor->albedoMap = new Texture("Assets/Textures/Plaster.jpg", true);
	floor->ao = vec3(0.01f);
	floor->metallic = 0;
	floor->roughness = 1.3f;
	floor->roughnessMap = new Texture("Assets/Textures/PlasterR.jpg", true);
	floor->TextureScale = vec2(1.0f);

	//PBRMaterial* beton = new PBRMaterial();
	//beton->albedo = vec3(1);
	//beton->albedoMap = new Texture("Assets/Textures/Plaster.jpg");
	//beton->ao = vec3(0.01f);
	//beton->metallic = 0;
	//beton->roughness = 0.4f;
	//beton->roughnessMap = new Texture("Assets/Textures/PlasterR.jpg");

	//PBRMaterial* grass = new PBRMaterial();
	//grass->albedo = vec3(1);
	//grass->albedoMap = new Texture("Assets/Textures/RockGrass.jpg");
	//grass->ao = vec3(0.01f);
	//grass->metallic = 0;
	//grass->roughness = 10.0f;
	//grass->TextureScale = vec2(50);
	//grass->roughnessMap = new Texture("Assets/Textures/RockSpec.jpg");


	//PBRMaterial* rust = new PBRMaterial();
	//rust->albedo = vec3(0.4);
	//rust->albedoMap = new Texture("Assets/Textures/r2albedo.jpg",true);
	//rust->ao = vec3(0.5);
	//rust->metallic = 1;
	//rust->metallicMap = new Texture("Assets/Textures/r2metal.jpg",true);
	//rust->roughness = 0.0f;
	//rust->roughnessMap = new Texture("Assets/Textures/r2rough.jpg",true);
	//rust->TextureScale = vec2(1.0f);
	cube->material = floor;
	tab->material = twood;

	PBRMaterial* lampmat = new PBRMaterial();


	sphere->material = lampmat;


	Asset* t = new Asset(vec3(0, 0, 0), vec3(1.4, .75,.8), 1000, assetShapes::cube);
	tab->attachTo(t);

	Lamp* l = new Lamp();
	Asset* b = new Asset();
	l->attachTo(b);
	l->color = vec3(6.0);
	b->scale = vec3(.10f);
	b->position = vec3(1.0f, 1.0f, 1.0f);
	b->OnTick = LampTick;
	sphere->attachTo(b);

	//Lamp* l2 = new Lamp();
	//Asset* b2 = new Asset();
	//l2->attachTo(b2);
	//l2->color = vec3(0.8f);
	//b2->scale = vec3(.10f);
	//b2->position = vec3(0.5f, 6, 1.0f);
	

	//Asset* t = new Asset(vec3(0, -10, 0), vec3(50, 10, 50), 0, assetShapes::cube);
	//Mesh* mesh = ter->meshes[0];
	//mesh->material = beton;
	//Texture* tt = new Texture("Assets/Textures/stoneHeight.jpg");
	//t->setHeightmapCollision("Assets/Textures/stoneHeight.jpg");
	//Terrain* terrain = new Terrain(mesh,tt);
	//terrain->material = grass;
	//terrain->grassMap = new Texture("Assets/Textures/RockSpec.jpg"); 
	//terrain->attachTo(t);

	// 400 baseline defered : 30 fps
	//for (int i = 0; i < 400; i++)
	//{
	//	Asset* a = new Asset(vec3(2, -0.5 + i* 0.1,-5), vec3(.040f), 10, assetShapes::ball);
	//	mpbr->attachTo(a);
	//}
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

	float radius = 3.0f;
	float lz = (float)sin(timerunning * 0.5f) * radius;
	float lx = (float)cos(timerunning *  0.5f) * radius;
	asset->setPosition(vec3(lx, 1.50f, lz));
}

void FallingTick(GLFWwindow * window, double deltaTime, Asset * asset)
{
	asset->setPosition(asset->position + vec3(0, 1, 0));
}
