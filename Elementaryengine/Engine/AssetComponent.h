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
	void attachTo(Asset* a);
	void detachFrom(Asset* a);

	vector<Asset*> parents;

};

