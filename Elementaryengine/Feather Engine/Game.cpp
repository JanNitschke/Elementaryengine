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

	displaySettings = new EDisplaySettings;
	displaySettings->windowname = name;
	eOpenGl->Initialise(displaySettings);
	return;

	//Setup Bullet physics
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -9.8, 0));


	// Setup components (shaders, textures etc.)

	Mesh::SetupMeshComp();
	Lamp::SetupLampComp();
	Asset::SetupAsset();
	
	LoadScene();


	float viewaspect = (float)displaySettings->windowWidth / (float)displaySettings->windowHeight;
	Projection = glm::perspective(glm::radians(60.0f), viewaspect, 0.1f, 100.0f);
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
			processInput(eOpenGl->window);
		}
		assets = nextAssets;
		View = activeCam->GetView();

		if (gameMode != nullptr) {
			gameMode->Tick(deltaTime);
		}
		for each (Asset* asset in assets)
		{
			asset->Tick(eOpenGl->window, deltaTime);
		}
		dynamicsWorld->stepSimulation(deltaTime, 1);

		if (!isServer) {
			RenderShadowMaps();
			Render();
			RenderHUD();

			// Swap buffers
			glfwSwapBuffers(eOpenGl->window);
			glfwPollEvents();
		}
		


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(eOpenGl->window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(eOpenGl->window) == 0);
}
bool Game::isKeyDown(int key)
{
	return (glfwGetKey(Instance().eOpenGl->window, key) == GLFW_PRESS);
}
void Game::setLight(vec3 color, vec3 direction)
{
	directionalLightColor = color;
	directionalLightDirection = direction;
}
void Game::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, eOpenGl->gBuffer	);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, displaySettings->windowWidth, displaySettings->windowHeight);
	
	// geometry pass
	Mesh::geometryShader->use();

	for each (Asset* a in assets)
	{
			a->Render(View, Projection);
	}

	// lighting pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Shader* shader = Mesh::pbrShader;
	shader->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gPosition);
	shader->setInt("gPosition", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gNormal);
	shader->setInt("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gAlbedoSpec);
	shader->setInt("gAlbedoSpec", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gMaterial);
	shader->setInt("gMaterial", 3);

	eSetLampsCommand(shader, eOpenGl->lightColorSSBO, eOpenGl->lightPositionSSBO);
	eOpenGl->renderQuad();


	// copy content of geometry's depth buffer to default framebuffer's depth buffer

	glBindFramebuffer(GL_READ_FRAMEBUFFER, eOpenGl->gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
											   // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
											   // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
											   // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
	glBlitFramebuffer(0, 0, displaySettings->windowWidth, displaySettings->windowHeight, 0, 0, displaySettings->windowWidth, displaySettings->windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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

