#pragma once
#include <string>
#include <vector>
#include <Asset.h>

using namespace std;
using namespace glm;

///<summary>
/// Contains assets. Can be loaded and unloaded to different points.
///</summary> 
class ELevel
{
public:
	ELevel();
	~ELevel();
	vector<Asset*> assets;

	void Load();
	void Unload();
	void Serialize(string path);
	void Deserialize(string path);
	void Destroy();
	
	void setPosition(vec3);
	vec3 getPosition();

	void setScale(vec3);
	vec3 getScale();

	void setRotation(quat);
	quat getRotation();

	bool isLoaded();
private:
	bool loaded = false;
	vec3 position = vec3(0);
	vec3 scale = vec3(1);
	quat rotation = quat();
};

