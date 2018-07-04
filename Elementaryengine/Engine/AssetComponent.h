#pragma once
#include <EEngine.h>
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
	virtual void AttachTo(Asset* a);
	virtual	void Detach();
	Asset* parent;
};

