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
#include "ToonDemoMode.h"
#include <GameMode.h>
#include <iostream>
#include <glm\glm.hpp>
int main()
{
	//char* name = "Feather Engine";
	Game game = Game::Instance();
	game.windowHeight = 900;
	game.windowWidth = 1600;
	game.name = "Toon demo";
	game.setIsServer(false);
	ToonDemoMode* mode = new ToonDemoMode();
	mode->game = &game;
	game.gameMode = mode;
	game.setLight(vec3(0.1f),vec3(0.1,0.2,0.3));
	game.Start();
	return 0;
}

