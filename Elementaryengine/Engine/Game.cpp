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

Texture * Game::loadTexture(const char * path)
{
	return renderer->loadTexture(path);
}

void Game::SetActiveCam(Camera * camera)
{
	Game::activeCam = camera;
}


bool Game::requireServer = false;
bool Game::isServer = false;
bool Game::physicsFinished = false;
bool Game::shouldClose = false;
vec3 Game::directionalLightColor;
vec3 Game::directionalLightDirection;

vector<Asset*> Game::assetsToDelete;

vector<Mesh*> Game::meshs;
vector<Lamp*> Game::lamps;
vector<ETextElement*> Game::textElements;
vector<ELevel*> Game::levels;
ELevel* Game::mainLevel = new ELevel();
Camera* Game::activeCam;
btDiscreteDynamicsWorld* Game::dynamicsWorld;
bool Game::simulatePhysics = true;
double Game::physicsFps;
EOpenGl* Game::eOpenGl = new EOpenGl();
bool Game::meshChanged = true;
vector<UIElement*> Game::uiElements;
vec2 Game::scroll;
bool Game::scrolledThisFrame = false;
ERenderer* Game::renderer;

double Game::deltaTime;
double Game::currentTime;
unsigned int Game::frameCount = 0;
double Game::smoothFps;

mat4 Game::View;
mat4 Game::Projection;
EDisplaySettings* Game::displaySettings = new EDisplaySettings();
EConsole Game::console = EConsole();
ENetworkInterface * Game::networkInterface;
// start the game and run the main loop
void Game::Start()
{
	displaySettings->windowname = name;
	
	eOpenGl->Initialise(displaySettings);

	eScriptContext = new EScriptContext();
	mainLevel->Load();

	//Setup Bullet physics
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
	
	networkInterface = new ENetworkInterface();

	system("cls");

	// Setup components (shaders, textures etc.)
	Shader::defines = renderer->getShaderDefines();
	Mesh::SetupMeshComp();
	Lamp::SetupLampComp();
	Asset::SetupAsset();
	renderer->Setup(eOpenGl,displaySettings);
	console.SetUp();

	gameMode->window = eOpenGl->window;

	LoadScene();
	eScriptContext->ReadScript(L"main.js");

	float viewaspect = (float)displaySettings->windowWidth / (float)displaySettings->windowHeight;
	Projection = glm::perspective(glm::radians(60.0f), viewaspect, 0.1f, 100.0f);

	// for an ortho camera :
	UIProjection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// projection for voxelisation NOT USED ATM
	VoxelProj = glm::ortho(-64.0f,64.0f,-64.0f,64.0f,-64.0f,64.0f); // In world coordinates

	loop();

	// cleanup physics, network and js
	delete dynamicsWorld;
	delete networkInterface;
	delete eScriptContext;

	// Close OpenGL window and terminate GLFW
	eOpenGl->CleanUp();


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
		frameCount++;
		printf(" \r fps smooth: %i loaded Levels: %i accurate: %f physics: %f", (int)(smoothFps + .5), (int)levels.size(), 1 / deltaTime, Game::physicsFps);
		if (!isServer) {
			processInput(eOpenGl->window);
		}
		View = activeCam->GetView();

		if (gameMode != nullptr) {
			gameMode->Tick(deltaTime);
		}
		eScriptContext->RunFunction("OnTick");
		for each (auto * level in levels)
		{
			if (level->isLoaded()) {
				for (auto i = level->assets.begin(); i != level->assets.end(); )
				{
					if ((*i)) (*i)->Tick(eOpenGl->window, deltaTime);
					i++;
				}
			}
		}
		scrolledThisFrame = false;

		// delete all Assets that were destroyed this frame
		for each (Asset* a in assetsToDelete)
		{
			ELevel* level = a->level;
			level->assets.erase(std::remove(level->assets.begin(), level->assets.end(), a), level->assets.end());
			Asset::rendererAssetChangedCallback(a);
			delete a;
		}
	
		assetsToDelete.clear();

		if (!isServer) {
			console.Update();
			Render();
			// Swap buffers
			glfwSwapBuffers(eOpenGl->window);
			glfwPollEvents();
		}
		if (isKeyDown(GLFW_KEY_GRAVE_ACCENT)) {
			if (!consoleKeyLastFrame) {
				console.Toggle();
				consoleKeyLastFrame = true;
			}
		}
		else {
			consoleKeyLastFrame = false;
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
void Game::Render()
{

	renderer->SetupFrame(meshChanged, eOpenGl);
	renderer->RenderFrame(eOpenGl, displaySettings, View, Projection);
	renderer->RenderFX(eOpenGl,displaySettings);

	meshChanged = false;
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
		for each (auto level in Game::levels) {
			for each(auto asset in level->assets) {
				if (asset->getRigidBody() == RayCallback.m_collisionObject) {
					r.hitAsset = asset;
					break;
				}
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
			Game::dynamicsWorld->stepSimulation(dTime, 1, 1.0f/30.0f);
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