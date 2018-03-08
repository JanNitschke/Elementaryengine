#pragma once

#include "GameMode.h"
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <Asset.h>
#include <Game.h>
#include "Astroid.h"

using namespace std;
using namespace glm;

class AstroidDemo : public GameMode
{
public:
	AstroidDemo();
	~AstroidDemo();

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

	vector<Astroid*> Astroids;
	vector<Mesh*> AstroidMeshs;
	vector<Model*> AstroidModels;
};

