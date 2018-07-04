#include "stdafx.h"
#include "AstroidDemo.h"
#include <EEngine.h>
#include <glm\glm.hpp>
#include <Model.h>
#include <Mesh.h>
#include <Lamp.h>
#include <FPCam.h>
#include <Terrain.h>


bool paused;
bool pause_pressedLastFrame = false;
#define M_PI 3.14159265358979323846  /* pi */


AstroidDemo::AstroidDemo()
{
}


AstroidDemo::~AstroidDemo()
{
}

void AstroidDemo::Tick(double deltaTime)
{
	if (Game::isKeyDown(GLFW_KEY_P)) {
		if (!pause_pressedLastFrame) {
			paused = !paused;
			Astroid::runOrbit = !Astroid::runOrbit;
		}
		pause_pressedLastFrame = true;
	}
	else {
		pause_pressedLastFrame = false;
	}
}

void AstroidDemo::Load()
{
	Game::simulatePhysics = false;
	FPCam* cam = new FPCam();
	cam->camSpeed = 0.5f;
	game->SetActiveCam(cam);
	Model* lamp = new Model("Assets/Meshs/Sphere.obj");
	Mesh* lam = lamp->meshes[0];



	PBRMaterial* astroidMat = new PBRMaterial();
	astroidMat->albedo = vec3(1);
	astroidMat->albedoMap = game->loadTexture("Assets/Textures/Plaster.jpg");
	astroidMat->ao = vec3(0.01f);
	astroidMat->metallic = 0;
	astroidMat->roughness = 1.0f;
	astroidMat->roughnessMap = game->loadTexture("Assets/Textures/PlasterR.jpg");
	astroidMat->TextureScale = vec2(1.0f);

	AstroidModels.push_back(new Model("Assets/Meshs/Astroid1.obj"));
	AstroidModels.push_back(new Model("Assets/Meshs/Astroid2.obj"));
	//AstroidModels.push_back(new Model("Assets/Meshs/Astroid3.obj"));
	//AstroidModels.push_back(new Model("Assets/Meshs/Astroid4.obj"));
	//AstroidModels.push_back(new Model("Assets/Meshs/Astroid5.obj"));

	for each (auto a in AstroidModels)
	{
		AstroidMeshs.push_back(a->meshes[0]);
		a->meshes[0]->material = astroidMat;
	}

	for (int i = 0; i < 100; i++)
	{
		float dof = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float rad = 10 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10)));
		float sc = 0.05 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.3))) * 0.1;
	
		float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2 * M_PI);
		float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2 * M_PI);
		float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2 * M_PI);
		float w = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 2 * M_PI);

		Astroid* a = new Astroid(vec3(0),vec3(1,1,1),0,assetShapes::ball);
		a->degOffset = dof;
		a->radius = rad;
		int j = rand() % 2;
		a->setScale(vec3(sc));
		a->setRotation(quat(w,x,y,z));
		AstroidMeshs[0]->AttachTo(a);
	}

	PBRMaterial* lampmat = new PBRMaterial();
	lampmat->albedo = vec3(1000.0f);

	lam->material = lampmat;

	Lamp* l = new Lamp();
	Asset* b = new Asset();
	l->AttachTo(b);
	l->color = vec3(30.0);
	b->scale = vec3(.10f);
	b->position = vec3(0,0,0);
	lam->AttachTo(b);

}

void AstroidDemo::Start()
{
}

void AstroidDemo::Stop()
{
}
