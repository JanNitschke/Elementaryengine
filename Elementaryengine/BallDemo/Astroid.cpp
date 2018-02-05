#include "stdafx.h"
#include "Astroid.h"

#define M_PI           3.14159265358979323846  /* pi */

bool Astroid::runOrbit = true;

Astroid::Astroid()
{
}

Astroid::Astroid(vec3 pos, vec3 scale, int mass, assetShapes shape)
{
	Asset(pos,scale,mass,shape);
}


Astroid::~Astroid()
{
}

void Astroid::Tick(GLFWwindow * window, double deltaTime)
{
	if (runOrbit) {
		timerunning += deltaTime;
		float speed = (10 - radius) / 1000;

		float lz = (float)sin(timerunning * speed + degOffset * 2 * M_PI) * radius;
		float lx = (float)cos(timerunning *  speed + degOffset * 2 * M_PI) * radius;
		setPosition(vec3(lx, 0, lz));
		Asset::Tick(window, deltaTime);
	}
}
