#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Feather.h>
#include <AssetComponent.h>
#include <Texture.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\Gimpact\btGImpactCollisionAlgorithm.h>

#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
using namespace glm;
using namespace std;
static void defaultOnTick(GLFWwindow * window, double deltaTime, Asset* asset){}
enum assetShapes{ball,cube};

class Asset
{
public:
	DllExport Asset();
	DllExport Asset(vec3 pos, vec3 scale, int mass, assetShapes shape);
	DllExport ~Asset();
	DllExport void setMass(float m);
	DllExport void setScale(vec3 sca);
	DllExport void setPosition(vec3 pos);
	DllExport void setFriction(float f);
	DllExport void setRotation(quat rot);
	DllExport void applyForce(vec3 force);
	DllExport void applyForce(vec3 forcepoint,vec3 force);
	DllExport void applyTorque(vec3 torque);

	DllExport void setCollisionSizeOffset(vec3 offset);
	DllExport void setCollisionPositionOffset(vec3 offset);

	DllExport btRigidBody* getRigidBody();

	float mass = 1;
	// Code to draw the asset, may get passed to the more specific class
	virtual void Render(mat4 view, mat4 projection);
	virtual void RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l);
	virtual void RenderEnvironmentMap(vector<mat4> view, mat4 projection);
	virtual void Render(vec3 pos, vec3 rot, vec3 scale, mat4 view, mat4 projection);
	// called ecery frame for game logic
	DllExport virtual void Tick(GLFWwindow * window, double deltaTime);
	DllExport void setTickFunction(void(*tickFunction)(GLFWwindow * window, double deltaTime, Asset* asset));
	void (*OnTick)(GLFWwindow * window, double deltaTime, Asset* asset);
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	Asset* parent;
	vector<AssetComponent*> components;
	vector<Asset*> children;
	Texture* environmentMap;
	vec3 collisionSizeOffset = vec3(1);
	vec3 collisionPosOffset = vec3(0);

	assetShapes assetShape = assetShapes::ball;
	bool renderEnvironment = false;
	static const unsigned int ENVIRONMENT_WIDTH, ENVIRONMENT_HEIGHT;
	static unsigned int envMapFBO;
	static void SetupAsset();
	quat q;

	DllExport void setHeightmapCollision(const char* path);
private:
	btDefaultMotionState* assetMotionState;
	btCollisionShape* btAssetShape;
	btRigidBody* assetRigidBody;
};

