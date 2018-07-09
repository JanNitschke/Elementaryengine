#pragma once
#include <GL/glew.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <stdlib.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>

class Asset;

namespace ElementaryEngine {

	enum Authority { None, Server, Client, Observer };
}