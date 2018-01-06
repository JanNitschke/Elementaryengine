#pragma once
#include <Feather.h>
#include <Shader.h>
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
using namespace glm;
using namespace std;
class DllExport AssetComponent
{
public:
	AssetComponent();
	~AssetComponent();
	void attachTo(Asset* a);
	void detachFrom(Asset* a);

	vector<Asset*> parents;
	virtual void Render(mat4 view, mat4 projection, Asset* parent, Shader* s);
	virtual void RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a);
};

