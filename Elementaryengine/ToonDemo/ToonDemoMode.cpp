#include "stdafx.h"
#include "ToonDemoMode.h"
#include <Feather.h>
#include <glm\glm.hpp>
#include <Model.h>
#include <Mesh.h>
#include <Lamp.h>
#include <FPCam.h>
#include <Terrain.h>
#include <GLFW\glfw3.h>
using namespace glm;
using namespace std;

double timerunning = 0;

ToonDemoMode::ToonDemoMode()
{
}

ToonDemoMode::~ToonDemoMode()
{
}

void ToonDemoMode::Tick(double deltaTime)
{
}

void ToonDemoMode::Load()
{
	FPCam* cam = new FPCam();
	cam->setPosition(vec3(0, 5, 0));
	game->SetActiveCam(cam);
	Model* ter = new Model("Assets/Meshs/terrain512.obj");

	Model* mCar = new Model("Assets/Meshs/car.obj");
	Model* mStreet = new Model("Assets/Meshs/Cube.obj");

	PBRMaterial* paint = new PBRMaterial();
	paint->albedo = vec3(40);
	paint->albedoMap = new Texture("Assets/Textures/empty.jpg");
	paint->ao = vec3(0.5f);
	paint->metallic = 0.0;
	paint->metallicMap = new Texture("Assets/Textures/empty.jpg");
	paint->roughness = 0.2;
	paint->TextureScale = vec2(0);
	paint->roughnessMap = new Texture("Assets/Textures/empty.jpg");

	PBRMaterial* interior = new PBRMaterial();
	interior->albedo = vec3(100);
	interior->albedoMap = new Texture("Assets/Textures/leather.jpg");
	interior->ao = vec3(0.01f);
	interior->metallic = 0;
	interior->roughness = 0.2;
	interior->TextureScale = vec2(10);
	interior->roughnessMap = new Texture("Assets/Textures/leatherr.jpg");

	PBRMaterial* breaks = new PBRMaterial();
	breaks->albedo = vec3(100,10,10);
	breaks->albedoMap = new Texture("Assets/Textures/empty.jpg");
	breaks->ao = vec3(0.01f);
	breaks->metallic = 1;
	breaks->roughness = 0;
	breaks->TextureScale = vec2(1);

	PBRMaterial* tires = new PBRMaterial();
	tires->albedo = vec3(5, 5, 5);
	tires->albedoMap = new Texture("Assets/Textures/empty.jpg");
	tires->ao = vec3(0.01f);
	tires->metallic = 0;
	tires->roughness = 0.2;
	tires->TextureScale = vec2(1);

	PBRMaterial* grass = new PBRMaterial();
	grass->albedo = vec3(100,100,100);
	grass->albedoMap = new Texture("Assets/Textures/RockGrass.jpg");
	grass->ao = vec3(0.01f);
	grass->metallic = 0;
	grass->roughness = 10.0f;
	grass->TextureScale = vec2(50);
	grass->roughnessMap = new Texture("Assets/Textures/RockSpec.jpg");

	PBRMaterial* road = new PBRMaterial();
	road->albedo = vec3(100);
	road->albedoMap = new Texture("Assets/Textures/road.jpg");
	road->ao = vec3(0.01f);
	road->metallicMap = new Texture("Assets/Textures/empty.jpg");
	road->metallic = 0;
	road->roughness = 0.4f;
	road->TextureScale = vec2(1,10);
	road->roughnessMap = new Texture("Assets/Textures/empty.jpg");

	DefaultMaterial* lampmat = new DefaultMaterial();
	lampmat->ambient = vec3(1000.0f);
	lampmat->diffuse = vec3(1.0f);
	lampmat->specular = vec3(1.0f);
	lampmat->shininess = 8;

	Asset* street = new Asset(vec3(0, 3.9, 0), vec3(2, 0.1, 50), 0, assetShapes::cube);
	mStreet->meshes[0]->material = road;
	mStreet->attachTo(street);
	street->setFriction(1);

	Asset* car = new Asset(vec3(0,8.2,0),vec3(1,1,1),100,assetShapes::cube);
	mCar->attachTo(car);
	mCar->meshes[0]->material = paint;
	mCar->meshes[1]->material = tires;
	mCar->meshes[2]->material = interior;
	mCar->meshes[3]->material = breaks;
	car->renderEnvironment = true;
	car->setCollisionSizeOffset(vec3(1,0.2,2));
	car->setCollisionPositionOffset(vec3(0, 0.3, 0));
	car->OnTick = CarTick;
	car->setFriction(0.5);
	car->getRigidBody()->setSpinningFriction(0.5);

	Asset* t = new Asset(vec3(0, -3.5, 0), vec3(50, 10, 50), 0, assetShapes::cube);
	Mesh* mesh = ter->meshes[0];
	mesh->material = grass;
	Texture* tt = new Texture("Assets/Textures/terrain.jpg");
	//t->setHeightmapCollision("Assets/Textures/terrain.jpg");
	t->setCollisionSizeOffset(vec3(0.001, 0.0001, 0.001));
	Terrain* terrain = new Terrain(mesh, tt);
	terrain->material = grass;
	terrain->grassMap = new Texture("Assets/Textures/RockSpec.jpg");
	terrain->attachTo(t);

	Model* ml = new Model("Assets/Meshs/Sphere.obj");
	Lamp* l = new Lamp();
	Asset* b = new Asset();
	l->attachTo(b);
	l->color = vec3(7);
	b->scale = vec3(.10f);
	b->position = vec3(1.0f, 1.0f, 1.0f);
	b->OnTick = LampTick;
	ml->attachTo(b);
	ml->meshes[0]->material = lampmat;

}

void ToonDemoMode::Start()
{
}

void ToonDemoMode::Stop()
{
}

void LampTick(GLFWwindow * window, double deltaTime, Asset * asset)
{
	timerunning += deltaTime;

	float radius = 6.0f;
	float lz = (float)sin(timerunning * 0.5f) * radius;
	float lx = (float)cos(timerunning *  0.5f) * radius;
	asset->setPosition(vec3(lx, 9.0f, lz));
}

void CarTick(GLFWwindow * window, double deltaTime, Asset * asset)
{
	if (Game::isKeyDown(GLFW_KEY_UP)) {
		asset->applyForce(vec3(0, 0, -1000));
	}
	if (Game::isKeyDown(GLFW_KEY_DOWN)) {
		asset->applyForce(vec3(0, 0, 1000));
	}
}
