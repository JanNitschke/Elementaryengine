#include "stdafx.h"
#include "BallDemoMode.h"
#include <Feather.h>
#include <glm\glm.hpp>
#include <Model.h>
#include <Mesh.h>
#include <Lamp.h>
#include <FPCam.h>
#include <Terrain.h>

#define M_PI           3.14159265358979323846  /* pi */


using namespace glm;
using namespace std;

double timerunning = 0;
bool p_pressedLastFrame = false;

BallDemoMode::BallDemoMode()
{
}

BallDemoMode::~BallDemoMode()
{
}

void BallDemoMode::Tick(double deltaTime)
{
	if (Game::isKeyDown(GLFW_KEY_P)) {
		if (!p_pressedLastFrame) {
			Game::simulatePhysics = !Game::simulatePhysics;
		}
		p_pressedLastFrame = true;
	}
	else {
		p_pressedLastFrame = false;
	}
}

void BallDemoMode::Load()
{
	Game::simulatePhysics = false;
	game->SetActiveCam(new FPCam());
	Model* m = new Model("Assets/Meshs/Cube.obj");
	Model* lamp = new Model("Assets/Meshs/Sphere.obj");
	Model* sp = new Model("Assets/Meshs/Cube.obj");

	Model* table = new Model("Assets/Meshs/tt.obj");

	Mesh* cube = m->meshes[0];
	Mesh* sphere = sp->meshes[0];
	Mesh* lam = lamp->meshes[0];
	Mesh* tab = table->meshes[0];

	Asset* a = new Asset(vec3(0,-0.9,0),vec3(50,0.1f,50),0,assetShapes::cube);
	a->setFriction(2);
	cube->attachTo(a);
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

	cube->material = floor;
	tab->material = twood;

	PBRMaterial* lampmat = new PBRMaterial();
	lampmat->albedo = vec3(1000.0f);

	lam->material = lampmat;

	sphere->material = pbrwood;


	Asset* t = new Asset(vec3(0, 0, 0), vec3(1.4, .75,.8), 1000, assetShapes::cube);
	tab->attachTo(t);

	Lamp* l = new Lamp();
	Asset* b = new Asset();
	l->attachTo(b);
	l->color = vec3(3.0);
	b->scale = vec3(.10f);
	b->position = vec3(1.0f, 1.0f, 1.0f);
	b->OnTick = LampTick;
	lam->attachTo(b);


	//400 baseline defered : 30 fps
	float height = -0.2f;
	float offset = 0.5f;
	for (int i = 0; i < 5000; i++)
	{
		float radius = 0.3f;
		float lz = (float)sin((i + offset) * (2.0f * M_PI)/ 12.0f) * radius;
		float lx = (float)cos((i + offset) *  (2.0f * M_PI)/ 12.0f) * radius;
		vec3 pos = vec3(lx + 2.0f,height - 0.5f,lz - 5);
		Asset* a = new Asset(pos, vec3(.040f), 1.1f, assetShapes::cube);
		vec3 rot = vec3((360 * (i % 12) / 12),0.1,0);
		a->setRotation(quat(rot));
		sphere->attachTo(a);
		a->setFriction(1.0f);
		height += (((i + 1) % 12) == 0)?0.081f:0.0f;
		
		if ((i + 1) % 12 == 0) {
			offset = (offset == 0.5f) ? 0.0f : 0.5f;
		}
	}
	height = 1.5f;
	for (int i = 0; i < 20; i++)
	{
		float radius = 1.5f;
		float lz = (float)sin(i * (2.0f * M_PI) / 20) * radius;
		float lx = (float)cos(i *  (2.0f * M_PI) / 20) * radius;
		vec3 pos = vec3(lx + 2.0f, height - 0.5f, lz - 5);
		Asset* a = new Asset(pos, vec3(.040f), 0, assetShapes::cube);
		Lamp* la = new Lamp();
		la->color = vec3(0.1f);
		lam->attachTo(a);
		la->attachTo(a);

	}
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

