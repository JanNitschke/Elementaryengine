#include "FPCam.h"
#include <glm/gtc/matrix_transform.hpp>



FPCam::FPCam()
{
	position = glm::vec3(3.0, 2.0, 7.0);
	cameraFront = glm::vec3(-0.3f, -0.2f, -0.7f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}


FPCam::~FPCam()
{
}

mat4 FPCam::GetView()
{
	vec3 front;
	front.x = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
	front.y = sin(glm::radians(rotation.x));
	front.z = cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
	cameraFront = glm::normalize(front);

	return lookAt(position, position + cameraFront, cameraUp);

}

void FPCam::Tick(GLFWwindow * window, double deltaTime)
{
	float cameraSpeed = camSpeed * deltaTime;
	float sensitivity = 0.05f;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);


	//Keypresses
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		position -= cameraSpeed * cameraUp;


	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}


	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates range from bottom to top
	lastX = (float)xpos;
	lastY = (float)ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;
	rotation.y += xoffset;
	rotation.x += yoffset;

	if (rotation.x  > 89.0f)
		rotation.x = 89.0f;
	if (rotation.x  < -89.0f)
		rotation.x = -89.0f;
	setPosition(position);
}
