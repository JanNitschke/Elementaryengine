#pragma once
#include "GameMode.h"
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <Asset.h>
#include <Game.h>

using namespace std;
using namespace glm;
class BallDemoMode : public GameMode
{
public:
	BallDemoMode();
	~BallDemoMode();
	Game* game;
	///<summary>
	///Called once per frame.
	///</summary> 
	///<param name="deltaTime">
	///Last frametime
	///</param>
	virtual void Tick(double deltaTime);

	///<summary>
	///Initial scene load. Called at the end of loading.
	///</summary> 
	virtual void Load();

	///<summary>
	///For your game logic at startup. Is called directly prior to the first frame.
	///</summary> 
	virtual void Start();

	///<summary>
	///Called when game stops.
	///</summary> 
	virtual void Stop();
	
private:

};
///<summary>
///Move the first lamp on tick. See Load() -  	b->OnTick = LampTick;
///</summary> 
void LampTick(GLFWwindow * window, double deltaTime, Asset* asset);

void FallingTick(GLFWwindow * window, double deltaTime, Asset* asset);
