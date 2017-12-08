#pragma once
#include <Feather.h>
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
	Asset* parent;
	virtual void Render(mat4 view, mat4 projection, Asset* parent);
	virtual void RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a);
};

