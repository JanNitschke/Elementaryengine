#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
class Asset;

namespace ElementaryEngine {
	//btVector3 toBullet(vec3 vec) {
	//	return btVector3(vec.x, vec.y, vec.z);
	//}
	//vec3 toGlm(btVector3 vec) {
	//	return vec3(vec.x, vec.y, vec.z);
	//}
	enum Authority { None, Server, Client, Observer };

}