#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <Game.h>

Camera::Camera()
{
	cameraUp = vec3(0, 1, 0);
	cameraFront = vec3(0, 1, 0);
}


Camera::~Camera()
{
}

glm::mat4 Camera::GetView()
{
	return lookAt(glm::vec3(camX, 0.0, camZ), vec3(0,0,0), cameraUp);
}

void Camera::Tick(GLFWwindow * window, double deltaTime)
{
	float radius = 10.0f;
	camX = (float)sin(glfwGetTime()) * radius;
	camZ = (float)cos(glfwGetTime()) * radius;
}
