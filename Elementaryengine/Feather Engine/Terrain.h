#include <Mesh.h>

class DllExport Terrain :
	public Mesh
{
public:
	Terrain();
	Terrain(Mesh* m, Texture* t);
	~Terrain();
	void SetupTerrain();
	Texture* heightmap;
	Texture* grassMap;
	virtual void Render(mat4 view, mat4 projection, Asset* parent);
	virtual void RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a);
	virtual void RenderEnvMap(vector<mat4> view, mat4 projection, Asset* a);
	GLuint VertexArrayID;
	GLuint vertexbuffer;
	static Shader* defaultShader;
	static Shader* lightmapShader;
	static Shader* pbrShader;
	static Shader* pbrEnvShader;

private:

};