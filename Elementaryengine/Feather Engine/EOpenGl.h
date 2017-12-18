#pragma once
#include <Feather.h>
#include <string>
#include <ERenderCommand.h>
#include <algorithm>
struct EDisplaySettings
{
	char* windowname;

	///<summary>
	/// window size
	///</summary> 
	int windowHeight = 900;
	int windowWidth = 1600;

};
struct DrawElementsIndirectCommand{
	GLuint  count;
	GLuint  primCount;
	GLuint  firstIndex;
	GLint   baseVertex;
	GLuint  baseInstance;
};
struct DrawMeshAtributes {
	mat4 Model;
	mat4 Rot;
	vec3 albedo;
	vec3 ao;
	float roughness;
	float metallic;
	int i0;
	int i1;
	int i3;
	int i4;
};

class EOpenGl
{
public:
	EOpenGl();
	~EOpenGl();
	// Gemetry Buffer 
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gColorSpec, gAlbedoSpec, gMaterial, gDepth;
	GLuint lightColorSSBO = 0;
	GLuint lightPositionSSBO = 0;
	GLuint meshDataSSBO = 0;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	// VXAO
	unsigned int vBuffer;
	unsigned int gridsize = 512;
	unsigned int vMap;

	void renderQuad();
	void Initialise(EDisplaySettings* settings);
	void CleanUp();
	///<summary>
	///The GLFW handle of the main window
	///</summary> 
	GLFWwindow* window;
};