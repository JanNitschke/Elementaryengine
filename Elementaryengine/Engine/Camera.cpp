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
	vec3 front;
	front.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
	front.y = sin(glm::radians(rotation.x));
	front.z = cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
	cameraFront = glm::normalize(front);
	return lookAt(position, position + cameraFront, cameraUp);
}

void Camera::Tick(GLFWwindow * window, double deltaTime)
{
	float radius = 10.0f;
	camX = (float)sin(glfwGetTime()) * radius;
	camZ = (float)cos(glfwGetTime()) * radius;
}
