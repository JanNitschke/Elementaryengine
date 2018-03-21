#include "stdafx.h"
#include "BallDemoMode.h"
#include <EEngine.h>
#include <glm\glm.hpp>
#include <Model.h>
#include <Mesh.h>
#include <Lamp.h>
#include <FPCam.h>
#include <Terrain.h>
#include <algorithm>

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
	vec3 dir = game->activeCam->cameraFront;
	vec3 pos = game->activeCam->position;
	RayCastHit r = game->Raycast(pos, pos + dir * 10.0f);
	vec3 result = r.hitPos;
	vec3 norm = r.hitNormal;

	// correct the normal  to 1 and 0;
	if (norm.x < 0.5f && norm.x > -0.5f) {
		norm.x = 0;
	} else {
		if (norm.x > 0.5f)
			norm.x = 1.0f;
		if (norm.x < 0.5f)
			norm.x = -1.0f;
	}

	if (norm.y < 0.5f && norm.y > -0.5f) {
		norm.y = 0;
	} else {
		if (norm.y > 0.5f)
			norm.y = 1.0f;
		if (norm.y < 0.5f)
			norm.y = -1.0f;
	}
		
	if (norm.z < 0.5f && norm.z > -0.5f) {
		norm.z = 0;
	} else {
		if (norm.z > 0.5f)
 			norm.z = 1.0f;
		if (norm.z < 0.5f)
			norm.z = -1.0f;
	}

	if (Game::isKeyDown(GLFW_MOUSE_BUTTON_1)) {
		if (placedLastFrame == false) {
			if (r.hitAsset != nullptr) {
				Asset* a;
				int db = 0;
				if (std::find(placedAssets.begin(), placedAssets.end(), r.hitAsset) != placedAssets.end()) {
					a = new Asset(r.hitAsset->position + (norm * 0.5f), vec3(0.25f), 0, assetShapes::cube);
					db = 1;
				}
				else {
					a = new Asset(result + (norm * 0.25f), vec3(0.25f), 0, assetShapes::cube);
					db = 2;
				}
				cube->attachTo(a);
				placedAssets.push_back(a);
			}
		}
		placedLastFrame = true;
	}
	else {
		placedLastFrame = false;
	}

	if (Game::isKeyDown(GLFW_MOUSE_BUTTON_2)) {
		if (destroyedLastFrame == false) {
			if (r.hitAsset != nullptr) {
				if (std::find(placedAssets.begin(), placedAssets.end(), r.hitAsset) != placedAssets.end()) {

					r.hitAsset->Destroy();
				}
			}
		}
		destroyedLastFrame = true;
	}
	else {
		destroyedLastFrame = false;
	}
}

void BallDemoMode::Load()
{
	Game::simulatePhysics = false;
	game->SetActiveCam(new FPCam());
	game->activeCam->setPosition(vec3(0,0,0));
	Model* m = new Model("Assets/Meshs/Cube.obj");
	Model* lamp = new Model("Assets/Meshs/Sphere.obj");
	Model* sp = new Model("Assets/Meshs/Cube.obj");
	Model * room = new Model("Assets/Meshs/Room.obj");
	Model* table = new Model("Assets/Meshs/tt.obj");
	Model* candleMod = new Model("Assets/Meshs/Candle.obj");

	cube = m->meshes[0];
	Mesh* sphere = sp->meshes[0];
	Mesh* lam = lamp->meshes[0];
	Mesh* tab = table->meshes[0];
	Mesh* ro = room->meshes[0];
	Mesh* cMesh = candleMod->meshes[0];

	Asset* a = new Asset(vec3(0,-0.9,0),vec3(50,0.1f,50),0,assetShapes::cube);
	a->setFriction(2);
	cube->attachTo(a);
	a->renderEnvironment = false;

	PBRMaterial* pbrwood = new PBRMaterial();
	pbrwood->albedo = vec3(1);
	pbrwood->albedoMap = game->loadTexture("Assets/Textures/Wood.jpg");
	pbrwood->ao = vec3(0.01f);
	pbrwood->metallic = 0;
	pbrwood->roughness = 0.4f;
	pbrwood->roughnessMap = game->loadTexture("Assets/Textures/WoodSpec.jpg");
	pbrwood->TextureScale = vec2(1.0f);


	PBRMaterial* twood = new PBRMaterial();
	twood->albedo = vec3(1);
	twood->albedoMap = game->loadTexture("Assets/Textures/tt.jpg");
	twood->ao = vec3(0.01f);
	twood->metallic = 0;
	twood->roughness = 3.0f;
	twood->roughnessMap = game->loadTexture("Assets/Textures/ttr.jpg");
	twood->TextureScale = vec2(1.0f);

	Texture* plaster = game->loadTexture("Assets/Textures/Plaster.jpg");

	PBRMaterial* floor = new PBRMaterial();
	floor->albedo = vec3(1);
	floor->albedoMap = plaster;
	floor->ao = vec3(0.01f);
	//floor->metallic = 0;
	//floor->roughness = 1.3f;
	floor->metallic = 0;
	floor->roughness = 5.0f;
	//floor->roughnessMap = new Texture("Assets/Textures/PlasterR.jpg", true);
	floor->TextureScale = vec2(1.0f);

	cube->material = floor;
	tab->material = twood;
	ro->material = floor;
	cMesh->material = floor;


	PBRMaterial* lampmat = new PBRMaterial();
	lampmat->albedo = vec3(1000.0f);

	lam->material = lampmat;

	sphere->material = pbrwood;


	Asset* t = new Asset(vec3(-2.0f, -0.20, -4.0f), vec3(1.4, .75,.8), 1000, assetShapes::cube);
	tab->attachTo(t);

	Asset* r = new Asset(vec3(-2.5, -1.0f, -2.5), vec3(4, 2, 4), 0, assetShapes::cube);
	ro->attachTo(r);

	Lamp* l = new Lamp();
	Asset* b = new Asset();
	l->attachTo(b);
	l->color = vec3(3.0,2.0,1.0);
	b->scale = vec3(.10f);
	b->position = vec3(-2.0f, 2.2f, -8.0f);
	//b->OnTick = LampTick;
	lam->attachTo(b);

	Lamp* ca = new Lamp();
	Asset* candle = new Asset();
	ca->attachTo(candle);
	ca->color = vec3(1.0, 0.6, 0.3);
	candle->scale = vec3(0.7,1.2,0.7);
	candle->position = vec3(-2.0, 0.9f, -4.0f);
	candle->OnTick = CandleTick;
	cMesh->attachTo(candle);
	cMesh->posOffset = vec3(0, -0.04, 0);

	//400 baseline defered : 30 fps
	float height = -0.2f;
	float offset = 0.5f;
	/*
	for (int i = 0; i < 10; i++)
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
	*/
	/*
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
	*/

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

void CandleTick(GLFWwindow * window, double deltaTime, Asset * asset) {
	Lamp* l = Game::lamps[1];
	timerunning += deltaTime;
	vec3 color = vec3(1.0, 0.6, 0.3);
	float baseint = 0.14f;
	float intensety = 0.2f;
	float i1 = ((float)sin(timerunning * 5) * intensety);
	float i2 = ((float)cos(timerunning *  7) * intensety);
	float i3 = ((float)cos(timerunning *  13) * intensety);

	l->color = color * (baseint + (i1 * i2 * i3));
}