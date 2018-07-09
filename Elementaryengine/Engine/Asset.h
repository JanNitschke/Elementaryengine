#pragma once
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>
#include <EEngine.h>
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

class ELevel;

class Asset
{
public:
	DllExport Asset();
	DllExport Asset(vec3 pos, vec3 scale, int mass, assetShapes shape);
	DllExport Asset(vec3 pos, vec3 scale, int mass, assetShapes shape, ELevel * level);
	virtual ~Asset();

	DllExport void setLevel(ELevel* level);
	DllExport void setMass(float m);
	DllExport virtual void setScale(vec3 sca);
	DllExport virtual void setPosition(vec3 pos);
	DllExport void setFriction(float f);
	DllExport virtual void setRotation(quat rot);
	DllExport void applyForce(vec3 force);
	DllExport void applyForce(vec3 forcepoint,vec3 force);
	DllExport void applyTorque(vec3 torque);
	DllExport virtual vec3 getPosition();
	DllExport virtual vec3 getScale();
	DllExport virtual quat getRotation();

	DllExport void setCollisionSizeOffset(vec3 offset);
	DllExport void setCollisionPositionOffset(vec3 offset);

	DllExport btRigidBody* getRigidBody();
	ELevel * level;

	float mass = 1;

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
	
	virtual void RenderGeometry();

	typedef void(*AssetCallback)(Asset* asset);

	static AssetCallback rendererAssetCreatedCallback;
	static AssetCallback rendererAssetChangedCallback;
	static AssetCallback rendererAssetDestroyedCallback;

	void Destroy();
	quat q;

	int renderPos;

	DllExport void setHeightmapCollision(const char* path);
private:
	btDefaultMotionState* assetMotionState;
	btCollisionShape* btAssetShape;
	btRigidBody* assetRigidBody;
};

