#pragma once
#include <EEngine.h>
#include <string>
#include <algorithm>
#include <glm\glm.hpp>
#include <Material.h>
#include <Shader.h>
#include <Mesh.h>


struct EDisplaySettings
{
	char* windowname;
	bool fullscreen = false;
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
	float roughness;
	vec3 ao;
	float metallic;
	int albedoTex;
	int metallicTex;
	int roughnessTex;
	int i0;
};
struct Light {
	vec3 pos;
	int throwShadow; // 0 -> no shadows; 1-> flat lighting; 3 -> volumetric 
	vec3 color;
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
	GLuint drawIdOffsetBuffer = 0;

	unsigned int lBuffer, frameOut;
	GLuint64 texAHandle;
	//multi draw indirect stuff
	unsigned int gVertexBuffer;
	unsigned int gArrayTexutre;
	unsigned int gElementBuffer;
	unsigned int gIndirectBuffer;
	GLuint vao;
	//Render Buffers vertex and index Buffer
	vector<Vertex> vVertex;
	vector<unsigned int>gIndex;

	// offset for Multidraw inderect instancing
	vector<int> drawInstanceOffset;
	int currentIndexOffset = 0;
	int currentVertexOffset = 0;
	int instance = 0;
	vector<DrawElementsIndirectCommand> dICommands;



	unsigned int shadowMaps;
	unsigned int textureArray;
	vector<int> freeLayers;

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

	// hold the indexes of the uniforms to avoid calling gl-get() 
	// geometry shader
	int geometryUniformVP = -1;
	int geometryUniformTextures = -1;

	// lighting shader
	int lightingUniformPosition = -1;
	int lightingUniformNormal = -1;
	int lightingUniformAlbedoSpec = -1;
	int lightingUniformMaterial = -1;
	int lightingUniformFar_plane = -1;
	int lightingUniformColorCorrection = -1;
	int lightingUniformShadowMaps = -1;
	int lightingUniformDepth = -1;
	int lightingUniformProj = -1;
	int lightingUniformView = -1;
	int lightingUniformViewPos = -1;

	// SSR shader
	int ssrUniformPosition = -1;
	int ssrUniformNormal = -1;
	int ssrUniformAlbedoSpec = -1;
	int ssrUniformMaterial = -1;
	int ssrUniformFar_plane = -1;
	int ssrUniformColor = -1;
	int ssrUniformDepth = -1;
	int ssrUniformViewPos = -1;
	int ssrUniformInvView = -1;
	int ssrUniformInvProj = -1;
	int ssrUniformView = -1;
	int ssrUniformProj = -1;

	// shadowmaps
	int shadowUniformLayer = -1;
	int shadowUniformShadowMatrices = -1;
	int shadowUniformFar_plane = -1;
	int shadoeUniformLightPos = -1;
};