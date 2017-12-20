#pragma once
#include <Mesh.h>
#include <Feather.h>
#include <Camera.h>
#include <Asset.h>
#include <Lamp.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\Gimpact\btGImpactCollisionAlgorithm.h>
#include <EOpenGl.h>

class GameMode;
#include <GameMode.h>
using namespace glm;
//
//#include "enet\enet.h"

#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
///<summary>
///Game class abstracts all things that belong to the game. Run Start() after creating an instance to run the game. Do not forget to set the window options and gamemode first.
///You can not create more than one instance of this class;
///</summary> 
struct DllExport RaycastHit {
	Asset* hit;
	vec3 hitPos;
};

class DllExport Game
{

public:
	static Game& Instance() { static Game game; return game; }

	///<summary>
	///DO NOT DESTROY THE GAME OBJECT WITHOUT CALLING Game::Stop() FIRST!
	///</summary> 
	~Game();
	unsigned int shadowMaps;
	static unsigned int textureArray;
	static const unsigned int TextureSize;
	static const unsigned int TextureCount;
	unsigned int lBuffer, frameOut;
	static vector<int> freeLayers;
	GLuint64 texAHandle;
	//multi draw indirect stuff
	unsigned int gVertexBuffer;
	unsigned int gArrayTexutre;
	unsigned int gElementBuffer;
	unsigned int gIndirectBuffer;
	unsigned int gDrawIdBuffer;
	GLuint vao;

	///<summary>
	/// Opengl functions
	///</summary> 
	EOpenGl* eOpenGl = new EOpenGl();

	EDisplaySettings* displaySettings;
	///<summary>
	///The Gamemode to load
	///</summary> 
	GameMode* gameMode;
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

	///<summary>
	///Assets to loop trough next frame. You may delete items from this, but the destructor usualy does this by itself.
	///</summary> 
	static vector<Asset*> nextAssets;

//TODO Fix ERROR if lamp has no parent
	///<summary>
	///Lamps to render (need to be atached to an asset or will throw error)
	///</summary> 
	static vector<Lamp*> lamps;

	///<summary>
	///Projection matrix for 3D scene;
	///</summary> 
	glm::mat4 Projection;

	///<summary>
	///Projection matrix for Voxelisation;
	///</summary> 
	glm::mat4 VoxelProj;

	int renderstate = 0;

	///<summary>
	///View matrix for the 3D scene;
	///</summary> 
	glm::mat4 View;

	///<summary>
	///Smoothed FPS counter
	///</summary> 
	double smoothFps;

	///<summary>
	///Time since last frame
	///</summary> 
	double deltaTime;

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
	///Draws all elements of the assets vector
	///</summary> 
	void SetupRender();

	///<summary>
	///Renders the shadow maps of all lights
	///</summary> 
	void RenderShadowMaps();

	///<summary>
	///Handles the input of the window and passes it to the active Gamemode. Not used atm
	///</summary> 
	///<param name="window">
	///The window to fetch the input from
	///</param>
	void processInput(GLFWwindow *window);


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
	static btDiscreteDynamicsWorld* dynamicsWorld;
	
	RaycastHit Raycast(vec3 Start, vec3 End);

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
	bool meshChanged = true;

private:
	///<summary>
	///Private constructor. Use shared_instance() to get the instance. Only one instance can exist simultaneously
	///</summary> 
	Game() {}

	///<summary>
	///Time since start of the game in ms
	///</summary> 
	double currentTime;

	///<summary>
	///Time from the start of the game to the last frame in ms
	///</summary> 
	double oldTime;
	/////<summary>
	/////If multiplayer what rights does this client have
	/////</summary> 
	//Authority authority;
	
	//Render Buffers etc.
	vector<Vertex> vVertex;
	vector<unsigned int>gIndex;
	int currentIndexOffset = 0;
	int currentVertexOffset = 0;
	int instance = 0;
	vector<DrawElementsIndirectCommand> dICommands;

};
 
