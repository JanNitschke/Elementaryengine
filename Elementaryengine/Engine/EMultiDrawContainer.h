#pragma once
#include <Mesh.h>
#include <vector>
#include <EMultiDrawMeshInstance.h>

using namespace std;
using namespace glm;

struct DrawElementsIndirectCommand {
	GLuint  count;
	GLuint  primCount;
	GLuint  firstIndex;
	GLint   baseVertex;
	GLuint  baseInstance;
};

class EMultiDrawContainer: 
	public Asset
{
public:
	vector<Mesh*> meshs; 
	vector<EMultiDrawMeshInstance*> instances;
	bool rebuildMesh, rebuildInstances = true;

	EMultiDrawContainer();
	~EMultiDrawContainer();

	GLuint vao;
	vector<Vertex> vertecies;
	vector<unsigned int> indices;
	vector<DrawElementsIndirectCommand> drawInderectCommands;
	vector<int> drawInstanceOffset;
	int currentIndexOffset = 0;
	int currentVertexOffset = 0;
	int instance = 0;


	void AddInstance(EMultiDrawMeshInstance * instance);
	void DeleteInstance(EMultiDrawMeshInstance * instance);
	void BuildMeshes();
	void BuildAssets();
};

