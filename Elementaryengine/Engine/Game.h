#pragma once
#include <Mesh.h>
#include <EEngine.h>
#include <Camera.h>
#include <Asset.h>
#include <Lamp.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\Gimpact\btGImpactCollisionAlgorithm.h>
#include <EOpenGl.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <UIElement.h>
#include <memory>
#include <EScriptContext.h>
#include <RayCastHit.h>
#include <ETextElement.h>
#include <EConsole.h>

class GameMode;
#include <GameMode.h>
using namespace glm;
class ERenderer;

class DllExport Game
{

public:
	static Game& Instance() { static Game game;
	return game; }

	///<summary>
	/// DO NOT DESTROY THE GAME OBJECT WITHOUT CALLING Game::Stop() FIRST!
	///</summary> 
	~Game();
	static bool shouldClose;
	static bool physicsFinished;

	///<summary>
	/// Opengl functions
	///</summary> 
	static EOpenGl* eOpenGl;

	static EDisplaySettings* displaySettings;
	///<summary>
	///The Gamemode to load
	///</summary> 
	GameMode* gameMode;

	EScriptContext* eScriptContext;

//Widow options. Permantent after Start() is called
	///<summary>
	///Name of the game and window title
	///</summary> 
	char* name ="Elementaryengine";

	///<summary>
	///Is this instance a client or a Server.
	///</summary> 
	static bool isServer;

	///<summary>
	///Use this to set the isServer variable
	///</summary> 
	void setIsServer(bool set);

	///<summary>
	///Is this a multiplayer Game that requires a server 
	///</summary> 
	static bool requireServer;

	static ERenderer* renderer;

	Texture* loadTexture(const char* path);

	///<summary>
	///The active camera
	///</summary> 
	static Camera* activeCam;

	///<summary>
	///Set the currently used camera
	///</summary> 
	void SetActiveCam(Camera* camera);

	///<summary>
	///Assets to loop trough this frame. DO NOT DELETE FROM THIS! Created assets register themselfs here.
	///</summary> 
	static vector<Asset*> assets;

	static vector <Mesh*> meshs;

	static vector <ETextElement*> textElements;
	
	///<summary>
	///Assets to loop trough next frame. You may delete items from this, but the destructor usualy does this by itself.
	///</summary> 
	static vector<Asset*> nextAssets;

	///<summary>
	///Assets to delete once the frame is completed
	///</summary> 
	static vector<Asset*> assetsToDelete;


	static vector<UIElement*> uiElements;

//TODO Fix ERROR if lamp has no parent
	///<summary>
	///Lamps to render (need to be atached to an asset or will throw error)
	///</summary> 
	static vector<Lamp*> lamps;

	///<summary>
	///Projection matrix for 3D scene;
	///</summary> 
	static glm::mat4 Projection;

	///<summary>
	///Projection matrix for UI;
	///</summary> 
	glm::mat4 UIProjection;

	///<summary>
	///Projection matrix for Voxelisation;
	///</summary> 
	glm::mat4 VoxelProj;

	int renderstate = 0;

	///<summary>
	///View matrix for the 3D scene;
	///</summary> 
	static glm::mat4 View;

	///<summary>
	///Smoothed FPS counter
	///</summary> 
	static double smoothFps;

	static double physicsFps;
	///<summary>
	///Time since last frame
	///</summary> 
	static double deltaTime;

	static unsigned int frameCount;

	///<summary>
	///Time since start of the game in ms
	///</summary> 
	static double currentTime;

	///<summary>
	///Starts the game and runs the main loop
	///</summary> 
	void Start();

	///<summary>
	///Stops the gameand exits main loop
	///</summary> 
	int Stop();

	///<summary>
	///Main loop of the game
	///</summary> 
	void loop();

	static bool isKeyDown(int key);

	static vec2 getScroll();

	///<summary>
	///Set the directional light color and direction
	///</summary> 
	void setLight(vec3 color, vec3 direction);
// Functions in the loop
	///<summary>
	///Draws all elements of the assets vector
	///</summary> 
	void Render();
	
	///<summary>
	///Handles the input of the window and passes it to the active Gamemode.
	///</summary> 
	///<param name="window">
	///The window to fetch the input from
	///</param>
	void processInput(GLFWwindow *window);

	void processPhysics();

	///<summary>
	///Renders the HUD
	///</summary> 
	void RenderHUD();

	///<summary>
	///Loads the Scene from the Gamemode
	///</summary> 
	void LoadScene();

//Directional Light default: no light;
	///<summary>
	///Color Of the directional light. default: vec3(0) for no light
	///</summary> 
	static vec3 directionalLightColor;
	///<summary>
	///Direction of the directional light
	///</summary> 
	static vec3 directionalLightDirection;

	

//Physics
	bool usePhysx;

	static btDiscreteDynamicsWorld* dynamicsWorld;
	static bool simulatePhysics;

	RayCastHit Raycast(vec3 Start, vec3 End);



//ENet Networking
	//ENetAddress address;
	//ENetHost * server;
	//ENetHost * client;

	///<summary>
	///Called inside the loop if multiplayer is enabled to send and recive network data.
	///</summary> 
	void updateNetwork();

	///<summary>
	///Connect to Server
	///</summary> 
	///<param name="ip">
	///adress to connect to
	///</param>
	void netConnect(string ip);

	///<summary>
	///Disconnect from Server
	///</summary> 
	void netDisconnect();

	//ENetPeer *peer;
	static bool meshChanged;

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static btVector3 toBullet(vec3);

	static vec3 toGlm(btVector3);

	static EConsole console;

private:
	///<summary>
	///Private constructor. Use shared_instance() to get the instance. Only one instance can exist simultaneously
	///</summary> 

	static vec2 scroll;
	static bool scrolledThisFrame;



	///<summary>
	///Time from the start of the game to the last frame in ms
	///</summary> 
	double oldTime;
	/////<summary>
	/////If multiplayer what rights does this client have
	/////</summary> 
	//Authority authority;
	
	bool consoleKeyLastFrame = false;
	

};
DWORD WINAPI PhysicsThread(LPVOID lpParam);

 
