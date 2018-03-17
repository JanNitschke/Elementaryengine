#include "Game.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
#include <string>
using namespace std;
#include <iostream>
#include <Model.h>
#include <Shader.h>
#include <FPCam.h>
#include <Lamp.h>
#include <iostream>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ERender.h>

//#include <enet\enet.h>

Game::~Game()
{
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}


void Game::setIsServer(bool set)
{
	isServer = set;
}

void Game::SetActiveCam(Camera * camera)
{
	Game::activeCam = camera;
}

const unsigned int Game::TextureSize = 1024;
const unsigned int Game::TextureCount = 64;
bool Game::requireServer = false;
bool Game::isServer = false;
bool Game::physicsFinished = false;
bool Game::shouldClose = false;
vec3 Game::directionalLightColor;
vec3 Game::directionalLightDirection;
vector<Asset*> Game::assets;
vector<Asset*> Game::nextAssets;
vector<Asset*> Game::assetsToDelete;
vector<Mesh*> Game::meshs;
vector<Lamp*> Game::lamps;
Camera* Game::activeCam;
btDiscreteDynamicsWorld* Game::dynamicsWorld;
bool Game::simulatePhysics = true;
double Game::physicsFps;
EOpenGl* Game::eOpenGl = new EOpenGl();
bool Game::meshChanged = true;
bool Game::assetsChanged = true;
vector<UIElement*> Game::uiElements;
vec2 Game::scroll;
bool Game::scrolledThisFrame = false;

// start the game and run the main loop
void Game::Start()
{
	displaySettings->windowname = name;
	
	eOpenGl->Initialise(displaySettings);


	eScriptContext = new EScriptContext();

	//Setup Bullet physics
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
	

	system("cls");

	// Setup components (shaders, textures etc.)
	gameMode->window = eOpenGl->window;

	Mesh::SetupMeshComp();
	Lamp::SetupLampComp();
	Asset::SetupAsset();

	glGenTextures(1, &eOpenGl->shadowMaps);
	glGenFramebuffers(1, &eOpenGl->lBuffer);
	glGenTextures(1, &eOpenGl->frameOut);

	glGenTextures(1, &Game::eOpenGl->textureArray);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, Game::eOpenGl->textureArray);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, TextureSize, TextureSize, TextureCount);
	//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, Game::TextureSize, Game::TextureSize, TextureCount,0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);



	for (int i = 0; i < TextureCount; i++)
	{
		eOpenGl->freeLayers.push_back(i);
	}
	new Texture("Assets/Textures/empty.jpg", true);
	// Setup Render buffers for mdei
	glGenVertexArrays(1, &eOpenGl->vao);
	glGenBuffers(1, &eOpenGl->gElementBuffer);
	glGenBuffers(1, &eOpenGl->gIndirectBuffer);
	glGenBuffers(1, &eOpenGl->gVertexBuffer);

	LoadScene();
	eScriptContext->ReadScript(L"main.js");

	float viewaspect = (float)displaySettings->windowWidth / (float)displaySettings->windowHeight;
	Projection = glm::perspective(glm::radians(60.0f), viewaspect, 0.1f, 100.0f);

	// for an ortho camera :
	UIProjection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// projection for voxelisation NOT USED ATM
	VoxelProj = glm::ortho(-64.0f,64.0f,-64.0f,64.0f,-64.0f,64.0f); // In world coordinates


	loop();

	// cleanup physics
	delete dynamicsWorld;

	// Close OpenGL window and terminate GLFW
	eOpenGl->CleanUp();

	delete eScriptContext;
}


// stop the game, exit main loop
int Game::Stop()
{
	return 0;
}

// main loop of the game
void Game::loop()
{
	if (gameMode != nullptr) {
		gameMode->Start();
	}
	currentTime = glfwGetTime();
	smoothFps = 60;
	shouldClose = false;
	DWORD   threadId;
	HANDLE  threadHandle;
	threadHandle = CreateThread(NULL, 0, PhysicsThread, NULL, 0, &threadId);
	bool ff = true;

	do {
		assets = vector<Asset*>(nextAssets);

		if (isServer || requireServer) {
			updateNetwork();
		}
		if(isServer){

		} else {
			glfwPollEvents();
		}
		oldTime = currentTime;
		currentTime = glfwGetTime();
		deltaTime = currentTime - oldTime;
		smoothFps = (9 * smoothFps / 10) + (.1 / deltaTime);
		printf(" \r fps smooth: %i loaded Assets: %i accurate: %f physics: %f ", (int)(smoothFps + .5), (int)assets.size(), 1 / deltaTime, Game::physicsFps);
		if (!isServer) {
			processInput(eOpenGl->window);
		}
		View = activeCam->GetView();

		if (gameMode != nullptr) {
			gameMode->Tick(deltaTime);
		}
		eScriptContext->RunFunction("OnTick");
		for each (Asset* asset in assets)
		{
			if (asset) {
				asset->Tick(eOpenGl->window, deltaTime);
			}
		}
		scrolledThisFrame = false;
		// delete all Assets that were destroyed this frame
		for each (Asset* a in assetsToDelete)
		{
			nextAssets.erase(std::remove(Game::nextAssets.begin(), Game::nextAssets.end(), a), Game::nextAssets.end());
			delete a;
		}
		assetsToDelete.clear();
		nextAssets.shrink_to_fit();

		if (!isServer) {
			SetupRender();
			RenderShadowMaps();
			Render();
			//RenderHUD();
			//RenderUI();

			// Swap buffers
			glfwSwapBuffers(eOpenGl->window);
			glfwPollEvents();
		}
		if (!(glfwGetKey(eOpenGl->window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(eOpenGl->window) == 0)){
			shouldClose = true;
		}
	} // Check if the ESC key was pressed or the window was closed
	while (!shouldClose);
	while (!physicsFinished)
	{
		Sleep(10);
	}
}
bool Game::isKeyDown(int key)
{
	return (glfwGetKey(Instance().eOpenGl->window, key) == GLFW_PRESS) || (glfwGetMouseButton(Instance().eOpenGl->window,key) == GLFW_PRESS);
}
vec2 Game::getScroll()
{
	return scrolledThisFrame?scroll:vec2(0);
}
void Game::setLight(vec3 color, vec3 direction)
{
	directionalLightColor = color;
	directionalLightDirection = direction;
}

void Game::SetupRender()
{
	// rebuild the render Mesh and / or renderCommandQueue if needed
	//ERender::BuildMeshes(assetsChanged, meshChanged, eOpenGl);
	ERender::BuildMeshes(assetsChanged, meshChanged, eOpenGl);

	// rebuild the List of draw atributes
	ERender::BuildDrawAtrib(eOpenGl);

	// reset the Variables used to notify about new changes
	assetsChanged = false;
	meshChanged = false;
}

void Game::Render()
{
	// render the frame using ElementaryRenderer
	ERender::RenderFrame(eOpenGl, displaySettings, View, Projection);
}

void Game::RenderShadowMaps()
{
	ERender::RenderShadowMaps(eOpenGl);
}

void Game::RenderUI()
{
	ERender::RenderShadowMaps(eOpenGl);
}

void Game::processInput(GLFWwindow * window)
{
	
}

void Game::processPhysics()
{
}

void Game::RenderHUD()
{

}

void Game::LoadScene()
{
	if (gameMode != nullptr) {
		gameMode->Load();
	}
}

RayCastHit Game::Raycast(vec3 Start, vec3 End)
{
	RayCastHit r;
	btCollisionWorld::ClosestRayResultCallback RayCallback(toBullet(Start), toBullet(End));
	//Perform raycast
	RayCallback.m_hitNormalWorld;
	dynamicsWorld->rayTest(toBullet(Start), toBullet(End), RayCallback);
	if (RayCallback.hasHit()) {
		r.hitPos = toGlm(RayCallback.m_hitPointWorld);
		r.hitNormal = toGlm(RayCallback.m_hitNormalWorld);
		for each (Asset* a in assets)
		{
			if (a->getRigidBody() == RayCallback.m_collisionObject) {
				r.hitAsset = a;
				break;
			}
		}
	}
	return r;
}

void Game::updateNetwork()
{
	
}

void Game::netConnect(string ip)
{
	
}

void Game::netDisconnect()
{

}

btVector3 Game::toBullet(vec3 v)
{
	return btVector3(v.x, v.y,v.z);
}
vec3 Game::toGlm(btVector3 v)
{
	return vec3( v.getX(), v.getY(), v.getZ());
}
DWORD WINAPI PhysicsThread(LPVOID lpParam)
{
	int i;
	DWORD idThread;

	idThread = GetCurrentThreadId();
	double oTime = 0;
	double cTime = 0;
	double dTime = 0;

	while (!Game::shouldClose)
	{
		oTime = cTime;
		cTime = glfwGetTime();
		dTime = cTime - oTime;
		if (Game::simulatePhysics) {
			Game::dynamicsWorld->stepSimulation(dTime, 1);
		}
		else {
			Sleep(100);
		}
		Game::physicsFps = 1 / dTime;
	}
	Game::physicsFinished = true;
	return 0;
}

void Game::scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	vec2 pos = vec2(xoffset, yoffset);
	scroll = pos;
	scrolledThisFrame = true;
}