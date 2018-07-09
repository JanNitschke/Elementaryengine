// "BallDemo.cpp": Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "stdafx.h"
#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <string>
using namespace glm;
using namespace std;
#include <Game.h>
#include "BallDemoMode.h"
#include <GameMode.h>
#include <iostream>
#include "AstroidDemo.h"
#include <EModularRasterizer.h>

int main()
{
	int a; 
	//cin >> a;

	Game game = Game::Instance();
	//game.renderer = new ERasterizer();
	Game::displaySettings->windowHeight = 900;
	Game::displaySettings->windowWidth = 1600;
	game.renderer = new EModularRasterizer();
	game.name = "Ball demo";
	BallDemoMode* mode = new BallDemoMode();
	mode->game = &game;
	game.displaySettings->fullscreen = false;
	game.setLight(vec3(0.02f), vec3(0.1, 0.2, 0.3));
	game.gameMode = mode;
	game.Start();

}

