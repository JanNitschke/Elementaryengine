#include "Game.h"

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <string>
using namespace std;
#include <iostream>
#include <Model.h>
#include <Shader.h>
#include <FPCam.h>
#include <Lamp.h>
//#include <enet\enet.h>
using namespace FeatherEngine;

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

bool Game::requireServer = false;
bool Game::isServer = false;
GLFWwindow* Game::window;
vec3 Game::directionalLightColor;
vec3 Game::directionalLightDirection;
vector<Asset*> Game::assets;
vector<Asset*> Game::nextAssets;
vector<Lamp*> Game::lamps;
Camera* Game::activeCam;
btDiscreteDynamicsWorld* Game::dynamicsWorld;

// start the game and run the main loop
void Game::Start()
{
	// if networking is needed setup enet
	if (isServer || requireServer) {
	/*	if (enet_initialize() != 0)
		{
			fprintf(stderr, "Failed to initialize GLFW\n");
			return;
		}*/
	}
	//test if its a server: 
	if (isServer) {

		///* Bind the server to the default localhost.     */
		///* A specific host address can be specified by   */
		///* enet_address_set_host (& address, "x.x.x.x"); */
		//address.host = ENET_HOST_ANY;
		///* Bind the server to port 1234. */
		//address.port = 1234;
		//server = enet_host_create(&address /* the address to bind the server host to */,
		//	32      /* allow up to 32 clients and/or outgoing connections */,
		//	2      /* allow up to 2 channels to be used, 0 and 1 */,
		//	0      /* assume any amount of incoming bandwidth */,
		//	0      /* assume any amount of outgoing bandwidth */);
		//if (server == NULL)
		//{
		//	fprintf(stderr,
		//		"An error occurred while trying to create an ENet server host.\n");
		//	exit(EXIT_FAILURE);
		//}
	} else {
		//if (requireServer) {
		//	ENetHost * client;
		//	client = enet_host_create(NULL /* create a client host */,
		//		1 /* only allow 1 outgoing connection */,
		//		2 /* allow up 2 channels to be used, 0 and 1 */,
		//		57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
		//		14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
		//	if (client == NULL)
		//	{
		//		fprintf(stderr,
		//			"An error occurred while trying to create an ENet client host.\n");
		//		exit(EXIT_FAILURE);
		//	}
		//}
		// Initialise GLFW
		if (!glfwInit())
		{
			fprintf(stderr, "Failed to initialize GLFW\n");
			getchar();
			return;
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MacOS
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		// Open a window and create its OpenGL context
		window = glfwCreateWindow(windowWidth, windowHeight, name, NULL, NULL);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (window == NULL) {
			fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. \n");
			getchar();
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(window);

		// Initialize GLEW
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			getchar();
			glfwTerminate();
			return;
		}

		// Ensure we can capture the escape key being pressed below
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

		// Dark background
		glClearColor(0.050f, 0.125f, 0.247f, 0);

		// Enable depth test and backface culling
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_MULTISAMPLE);
	}



	//Setup Bullet physics
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -9.8, 0));



	// Setup components (shaders, textures etc.)
	if (!isServer) {
		Mesh::SetupMeshComp();
		Lamp::SetupLampComp();
		Asset::SetupAsset();
	}

	LoadScene();

	if (!isServer) {
		View = activeCam->GetView();
		float viewaspect = (float)windowWidth / (float)windowHeight;
		Projection = glm::perspective(glm::radians(60.0f), viewaspect, 0.1f, 100.0f);
	}
	//For an ortho camera :
	//Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates


	loop();

	//Cleanup physics
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;

	//cleanup ENet 
	/*atexit(enet_deinitialize);
	if (isServer) {
		enet_host_destroy(server);
	}*/

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
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
		printf(" \r fps smooth: %i accurate: %f ", (int)(smoothFps + .5), 1 / deltaTime);
		if (!isServer) {
			processInput(window);
		}
		assets = nextAssets;
		View = activeCam->GetView();

		if (gameMode != nullptr) {
			gameMode->Tick(deltaTime);
		}
		for each (Asset* asset in assets)
		{
			asset->Tick(window, deltaTime);
		}
		dynamicsWorld->stepSimulation(deltaTime, 1);

		if (!isServer) {
			RenderShadowMaps();
			RenderEnvironmentMaps();
			Render();
			RenderHUD();


			// Swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
}
bool Game::isKeyDown(int key)
{
	return (glfwGetKey(window, key) == GLFW_PRESS);
}
void Game::setLight(vec3 color, vec3 direction)
{
	directionalLightColor = color;
	directionalLightDirection = direction;
}
void Game::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);

	for each (Asset* a in assets)
	{
			a->Render(View, Projection);
	}
}

void Game::RenderEnvironmentMaps()
{
	glViewport(0, 0, Asset::ENVIRONMENT_WIDTH, Asset::ENVIRONMENT_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, Asset::envMapFBO);
	for each (Asset* asset in assets)
	{
		if (asset->renderEnvironment) {
			float aspect = (float)Asset::ENVIRONMENT_WIDTH / (float)Asset::ENVIRONMENT_HEIGHT;
			float Enear = 0.10f;
			float Efar = 25.0f;
			mat4 envProj = glm::perspective(glm::radians(90.0f), aspect, Enear, Efar);
			vec3 aPos = asset->position;
			vector<mat4> envTransforms;
			envTransforms.push_back(envProj *
				glm::lookAt(aPos, aPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			envTransforms.push_back(envProj *
				glm::lookAt(aPos, aPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			envTransforms.push_back(envProj *
				glm::lookAt(aPos, aPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
			envTransforms.push_back(envProj *
				glm::lookAt(aPos, aPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
			envTransforms.push_back(envProj *
				glm::lookAt(aPos, aPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			envTransforms.push_back(envProj *
				glm::lookAt(aPos, aPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, asset->environmentMap->id, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, asset->environmentMap->id);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			int i = 0;
			for each (Asset* a in assets)
			{
				if (a != asset) {
					a->RenderEnvironmentMap(envTransforms, mat4(1));
				}
			}
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Game::RenderShadowMaps()
{
	glViewport(0, 0, Lamp::SHADOW_WIDTH, Lamp::SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, Lamp::depthMapFBO);
	for each (Lamp* l in lamps)
	{
		float aspect = (float)Lamp::SHADOW_WIDTH / (float)Lamp::SHADOW_WIDTH;
		float Snear = 1.0f;
		float Sfar = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, Snear, Sfar);
		vec3 lightPos = l->parent->position;
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, l->depthmap->id, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, l->depthmap->id);
		glClear(GL_DEPTH_BUFFER_BIT);
		int i = 0;
		for each (Asset* a in assets)
		{
				a->RenderLightmap(shadowTransforms, mat4(1), l);
		}
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_2D, 0);
	}	
}

void Game::processInput(GLFWwindow * window)
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

RaycastHit Game::Raycast(vec3 Start, vec3 End)
{
	RaycastHit r;
	//btCollisionWorld::ClosestRayResultCallback RayCallback(toBullet(Start), toBullet(End));
	//// Perform raycast
	//dynamicsWorld->rayTest(toBullet(Start), toBullet(End), RayCallback);
	//if (RayCallback.hasHit()) {
	////	r.hitPos = RayCallback.;
	return r;
}

void Game::updateNetwork()
{
	//ENetEvent event;
	///* Wait up to 1000 milliseconds for an event. */
	//while (enet_host_service(client, &event, 1000) > 0)
	//{
	//	switch (event.type)
	//	{
	//	case ENET_EVENT_TYPE_CONNECT:
	//		printf("A new client connected from %x:%u.\n",
	//			event.peer->address.host,
	//			event.peer->address.port);
	//		/* Store any relevant client information here. */
	//		event.peer->data = "Client information";
	//		break;
	//	case ENET_EVENT_TYPE_RECEIVE:
	//		printf("A packet of length %u containing %s was received from %s on channel %u.\n",
	//			event.packet->dataLength,
	//			event.packet->data,
	//			event.peer->data,
	//			event.channelID);
	//		/* Clean up the packet now that we're done using it. */
	//		enet_packet_destroy(event.packet);

	//		break;

	//	case ENET_EVENT_TYPE_DISCONNECT:
	//		printf("%s disconnected.\n", event.peer->data);
	//		/* Reset the peer's client information. */
	//		event.peer->data = NULL;
	//	}
	//}
}

void Game::netConnect(string ip)
{
	//ENetAddress address;
	//ENetEvent event;
	///* Connect to some.server.net:1234. */
	//enet_address_set_host(&address, ip.c_str());
	//address.port = 1234;
	///* Initiate the connection, allocating the two channels 0 and 1. */
	//peer = enet_host_connect(client, &address, 2, 0);
	//if (peer == NULL)
	//{
	//	fprintf(stderr,
	//		"No available peers for initiating an ENet connection.\n");
	//	exit(EXIT_FAILURE);
	//}
	///* Wait up to 5 seconds for the connection attempt to succeed. */
	//if (enet_host_service(client, &event, 5000) > 0 &&
	//	event.type == ENET_EVENT_TYPE_CONNECT)
	//{
	//	puts("Connection to some.server.net:1234 succeeded.");
	//}
	//else
	//{
	//	/* Either the 5 seconds are up or a disconnect event was */
	//	/* received. Reset the peer in the event the 5 seconds   */
	//	/* had run out without any significant event.            */
	//	enet_peer_reset(peer);
	//	puts("Connection to some.server.net:1234 failed.");
	//}
}

void Game::netDisconnect()
{
//	ENetEvent event;
//	enet_peer_disconnect(peer, 0);
//	/* Allow up to 3 seconds for the disconnect to succeed
//	* and drop any packets received packets.
//	*/
//	while (enet_host_service(client, &event, 3000) > 0)
//	{
//		switch (event.type)
//		{
//		case ENET_EVENT_TYPE_RECEIVE:
//			enet_packet_destroy(event.packet);
//			break;
//		case ENET_EVENT_TYPE_DISCONNECT:
//			puts("Disconnection succeeded.");
//			return;
//		}
//	}
//	/* We've arrived here, so the disconnect attempt didn't */
//	/* succeed yet.  Force the connection down.             */
//	enet_peer_reset(peer);
}

