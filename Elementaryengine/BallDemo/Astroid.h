#pragma once

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <Asset.h>
#include <Game.h>

class Astroid : public Asset
{
public:
	Astroid();
	Astroid(vec3 pos, vec3 scale, int mass, assetShapes shape);

	~Astroid();
	
	static bool runOrbit;

	float radius;
	double timerunning;
	float degOffset;

	virtual void Tick(GLFWwindow * window, double deltaTime);

};

