#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
using namespace glm;
using namespace std;
#include <Game.h>

int main(void)
{
	//char* name = "Feather Engine";
	Game game = Game::Instance();
	game.windowHeight = 900;
	game.windowWidth = 1600;
	game.Start();
	return 0;
}

