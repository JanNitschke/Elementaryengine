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
int main()
{
	int a; 
	//cin >> a;
	//char* name = "Feather Engine";
	Game game = Game::Instance();
	game.name = "Ball demo";
	BallDemoMode* mode = new BallDemoMode();
	mode->game = &game;
	game.displaySettings->fullscreen = false;
	game.setLight(vec3(0.02f), vec3(0.1, 0.2, 0.3));
	game.gameMode = mode;
	game.Start();
	return 0;
}

