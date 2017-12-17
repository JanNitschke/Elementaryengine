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
#include <iostream>
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
	glGenTextures(1, &shadowMaps);
	glGenFramebuffers(1, &lBuffer);
	glGenTextures(1, &frameOut);
	LoadScene();



	float viewaspect = (float)displaySettings->windowWidth / (float)displaySettings->windowHeight;
	Projection = glm::perspective(glm::radians(60.0f), viewaspect, 0.1f, 100.0f);
	//For an ortho camera :
	//Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	VoxelProj = glm::ortho(-64.0f,64.0f,-64.0f,64.0f,-64.0f,64.0f); // In world coordinates


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
		//	SetupRender();
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
	//// voxel pass
	//Shader* s = Mesh::voxelShader;
	//s->use();
	//glBindFramebuffer(GL_FRAMEBUFFER, eOpenGl->vBuffer);
	////glEnable(GL_BLEND);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glViewport(0, 0, eOpenGl->gridsize, eOpenGl->gridsize);
	//eSetLampsCommand(s, eOpenGl->lightColorSSBO, eOpenGl->lightPositionSSBO);
	/*
	for each (Asset* a in assets)
	{
		a->Render(View, Projection, s);
	}
	*/
	// geometry pass
	glBindFramebuffer(GL_FRAMEBUFFER, eOpenGl->gBuffer);
	glDisable(GL_BLEND);
	glClearColor(0.050f, 0.125f, 0.247f, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, displaySettings->windowWidth, displaySettings->windowHeight);
	Mesh::geometryShader->use();

	for each (Asset* a in assets)
	{
			a->Render(View, Projection, Mesh::geometryShader);
	}

	// lighting pass	
	glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);

	glBindTexture(GL_TEXTURE_2D, frameOut);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, displaySettings->windowWidth, displaySettings->windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameOut, 0);

	Shader* shader = Mesh::pbrShader;
	shader->use();
	glDisable(GL_DEPTH_TEST);

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
	shader->setFloat("far_plane", 25);

	shader->setInt("colorCorrection",4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Mesh::colorCorrection->id);

	eSetLampsCommand(shader, eOpenGl->lightColorSSBO, eOpenGl->lightPositionSSBO);

	eOpenGl->renderQuad();



	//post fx 	
	glBlitFramebuffer(0, 0, displaySettings->windowWidth, displaySettings->windowHeight, 0, 0, displaySettings->windowWidth, displaySettings->windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	shader = Mesh::ssrShader;
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
	shader->setFloat("far_plane", 25);

	shader->setInt("colorCorrection", 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Mesh::colorCorrection->id);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, frameOut);
	shader->setInt("gColor", 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gDepth);
	shader->setInt("gDepth", 6);


	eSetLampsCommand(shader, eOpenGl->lightColorSSBO, eOpenGl->lightPositionSSBO);
	shader->set3Float("viewPos", activeCam->position);
	shader->setMat4f("invView", inverse(View));
	shader->setMat4f("invProj", inverse(Projection));
	shader->setMat4f("view", View);
	shader->setMat4f("proj", Projection);

	eOpenGl->renderQuad();



	// copy content of geometry's depth buffer to default framebuffer's depth buffer

	glBindFramebuffer(GL_READ_FRAMEBUFFER, eOpenGl->gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
											   // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
											   // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
											   // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
	glEnable(GL_DEPTH_TEST);

}

void Game::SetupRender()
{
	vector<Vertex> vVertex;
	vector<unsigned int>gIndex;
	int currentIndexOffset = 0;
	int currentVertexOffset = 0;
	int instance = 0;
	vector<DrawElementsIndirectCommand> dICommands;
	for each (Asset* a in assets)
	{
		for each (AssetComponent* c in a->components)
		{
			if (Model* d = dynamic_cast<Model*>(c)) {
				for each(Mesh* m in d->meshes) {
					// Reserve storage for Indices
					gIndex.reserve(gIndex.size() + m->indices.size());
					// Append Indices
					gIndex.insert(gIndex.end(), m->indices.begin(), m->indices.end());
					
					// Reserve storage for Indices
					vVertex.reserve((vVertex.size() + m->vertices.size()) * sizeof(Vertex));
					// Append Indices
					vVertex.insert(vVertex.end(), m->vertices.begin(), m->vertices.end());
					DrawElementsIndirectCommand c = DrawElementsIndirectCommand();
					c.count = m->vertices.size();
					// TODO: change this later for grouping mesh copys
					c.primCount = 1;
					c.firstIndex = currentIndexOffset;
					c.baseVertex = currentVertexOffset;
					c.baseInstance = instance;
					instance++;
					currentIndexOffset += m->indices.size();
					currentVertexOffset += m->vertices.size();
				}
			}
		}
	}
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Create a vertex buffer object
	glGenBuffers(1, &gVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertex), &vVertex, GL_STATIC_DRAW);


	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));


	//Create an element buffer
	glGenBuffers(1, &gElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gIndex), &gIndex, GL_STATIC_DRAW);

	glGenBuffers(1, &gIndirectBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gIndirectBuffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(dICommands), &dICommands, GL_STATIC_DRAW);

	GLuint vDrawId[100];
	for (GLuint i(0); i<100; i++)
	{
		vDrawId[i] = i;
	}

	glGenBuffers(1, &gDrawIdBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gDrawIdBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vDrawId), vDrawId, GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, (GLvoid*)0);
	glVertexAttribDivisor(3, 1);

}

void Game::RenderShadowMaps()
{
	Shader* shader = Mesh::lightmapShader;
	shader->use();
	int lightcount = lamps.size();
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMaps);

	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_RGBA16F, Lamp::SHADOW_WIDTH, Lamp::SHADOW_HEIGHT, 6 * lightcount, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glViewport(0, 0, Lamp::SHADOW_WIDTH, Lamp::SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, Lamp::depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowMaps, 0);

	glClearColor(1000,1000,1000,1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowMaps);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	int count = 0;
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

		shader->setInt("layer", count);

		count++;
	
		int i = 0;
		vector<Asset*> relevantAssets(assets.size());
		vec3 ppos = l->parent->position;
		auto it = std::copy_if(assets.begin(), assets.end(), relevantAssets.begin(), [&ppos](Asset* a) {return (length(a->position - ppos) < 25.0f) ; });
		relevantAssets.resize(std::distance(relevantAssets.begin(), it));  // shrink container to new size

		for each (Asset* a in relevantAssets)
		{
				a->RenderLightmap(shadowTransforms, mat4(1), l);
		}

	}	
	glBindTexture(GL_TEXTURE_2D, 0);
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

