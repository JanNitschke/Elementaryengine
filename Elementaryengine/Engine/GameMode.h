#pragma once
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
class Game;
#include <Game.h>

///<summary>
///Interface to Gamemodes. Gamemodes should handle all the scene loading and the global game logic.
///</summary> 
class DllExport GameMode {
public:
	///<param name="deltaTime">
	///Current game instance. Has to be set manualy if needed
	///</param>
	Game* game;

	GLFWwindow* window;
	///<summary>
	///Called once per frame.
	///</summary> 
	virtual void Tick(double deltaTime) = 0;

	///<summary>
	///Initial scene load. Called at the end of loading.
	///Make sure to create at least one camera and set Game::activeCam 
	///</summary> 
	virtual void Load() = 0;

	///<summary>
	///For your game logic at startup. Is called directly prior to the first frame.
	///</summary> 
	virtual void Start() = 0;

	///<summary>
	///Called when game stops.
	///</summary> 
	virtual void Stop() = 0;


};