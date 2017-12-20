#pragma once
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <Feather.h>


using namespace glm;
using namespace std;



class Shader
{
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* geompath, const GLchar* fragmentPath);

	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void set2Float(const std::string &name, glm::vec2 value) const;
	void set3Float(const std::string &name, glm::vec3 value) const;
	void setMat4f(const std::string &name, glm::mat4 value) const;
private:
	void checkCompileErrors(unsigned int shader, std::string type);

};

