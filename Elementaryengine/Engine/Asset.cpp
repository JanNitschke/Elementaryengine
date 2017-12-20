#include "Asset.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Game.h"
#include <BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h>

const unsigned int Asset::ENVIRONMENT_WIDTH = 1024, Asset::ENVIRONMENT_HEIGHT = 1024;
unsigned int Asset::envMapFBO;

Asset::Asset()
{



	scale = vec3(1.0f);

	Game::nextAssets.push_back(this);
	OnTick = &defaultOnTick;

}
Asset::Asset(vec3 pos, vec3 scale, int mass, assetShapes shape)
{
	
	

	Game::nextAssets.push_back(this);
	OnTick = &defaultOnTick;

	this->scale = scale;
	position = pos;
	assetShape = shape;
	if (assetShape == assetShapes::ball) {
		btAssetShape = new btSphereShape(scale.x);
	}
	else {
		btAssetShape = new btBoxShape(btVector3(scale.x * collisionSizeOffset.x,scale.y* collisionSizeOffset.y,scale.z* collisionSizeOffset.z));
	}
	btVector3 inertia(1, 1, 1);
	btAssetShape->calculateLocalInertia(mass, inertia);
	assetMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x + collisionPosOffset.x, position.y + collisionPosOffset.y, position.z + collisionPosOffset.z)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(mass, assetMotionState, btAssetShape, inertia);
	assetRigidBody = new btRigidBody(groundRigidBodyCI);
	Game::dynamicsWorld->addRigidBody(assetRigidBody);
	assetRigidBody->setFriction(0.4);
}

Asset::~Asset()
{
	Game::nextAssets.erase(std::remove(Game::nextAssets.begin(), Game::nextAssets.end(), this), Game::nextAssets.end());
}

DllExport void Asset::setMass(float m)
{
	mass = m;
	if (assetRigidBody != nullptr) {
		btVector3 inertia(0, 0, 0);
		assetRigidBody->setMassProps(m, inertia);
	}
}

DllExport void Asset::setScale(vec3 sca)
{
	scale = sca;
	if (assetRigidBody != nullptr) {
		if (assetShape == assetShapes::ball) {
			btAssetShape = new btSphereShape(scale.x * collisionSizeOffset.x);
		}
		else {
			btAssetShape = new btBoxShape(btVector3(scale.x * collisionSizeOffset.x, scale.y * collisionSizeOffset.y, scale.z* collisionSizeOffset.z));
		}
		assetRigidBody->setCollisionShape(btAssetShape);
	}
}

DllExport void Asset::setPosition(vec3 pos)
{
	position = pos;
	if (assetRigidBody != nullptr) {
		btTransform trans;
		trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
		trans.setOrigin(btVector3(pos.x + collisionPosOffset.x, pos.y + collisionPosOffset.y, pos.z + collisionPosOffset.z));
		assetRigidBody->setWorldTransform(trans);
	}
}

DllExport void Asset::setFriction(float f)
{
	assetRigidBody->setFriction(f);
}

DllExport void Asset::setRotation(quat rot)
{
	q = rot;
}

DllExport void Asset::applyForce(vec3 force)
{
	assetRigidBody->activate(true);
	assetRigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));
}

DllExport void Asset::applyForce(vec3 forcepoint, vec3 force)
{
	assetRigidBody->activate(true);
	assetRigidBody->applyForce(btVector3(force.x, force.y, force.z), btVector3(forcepoint.x, forcepoint.y, forcepoint.z));
}

DllExport void Asset::applyTorque(vec3 torque)
{	
	assetRigidBody->applyTorqueImpulse(btVector3(torque.x, torque.y, torque.z));
}

DllExport void Asset::setCollisionSizeOffset(vec3 offset)
{
	collisionSizeOffset = offset;
	setScale(scale);
}

DllExport void Asset::setCollisionPositionOffset(vec3 offset)
{
	collisionPosOffset = offset;
}

DllExport btRigidBody * Asset::getRigidBody()
{
	return assetRigidBody;
}


void Asset::Render(mat4 view, mat4 projection, Shader* s)
{
	Render(position,  rotation, scale,  view,  projection,s);
}

void Asset::RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l)
{
	for each (AssetComponent* c in components)
	{
		c->RenderLightmap( view, projection, l,this);
	}
}



void Asset::Render(vec3 pos, vec3 rot, vec3 scale, mat4 view, mat4 projection, Shader* s)
{
	for each (AssetComponent* c in components)
	{
		c->Render(view, projection, this,s);
	}
}
// called every frame for game logic
void Asset::Tick(GLFWwindow * window, double deltaTime)
{
	if (assetRigidBody != nullptr) {
		btTransform trans;
		assetRigidBody->getMotionState()->getWorldTransform(trans);
		position.x = trans.getOrigin().getX() - collisionPosOffset.x;
		position.y = trans.getOrigin().getY() - collisionPosOffset.y;
		position.z = trans.getOrigin().getZ() - collisionPosOffset.z;

		q.w = trans.getRotation().getW();
		q.x = trans.getRotation().getX();
		q.y = trans.getRotation().getY();
		q.z = trans.getRotation().getZ();

	}
	OnTick(window, deltaTime,this);
}

DllExport void Asset::setTickFunction(void(*tickFunction)(GLFWwindow *window, double deltaTime, Asset* asset))
{
	OnTick = tickFunction;
}

void Asset::SetupAsset()
{

}

void Asset::setHeightmapCollision(const char * path)
{
	int width, height, nrChannels;
	unsigned char* data = Texture::LoadPicture(path);
	btAssetShape = new btHeightfieldTerrainShape(512, 512, data, scale.y, 2, false, false);
	btAssetShape = new btHeightfieldTerrainShape(512, 512, data,1,0,1,1,PHY_ScalarType::PHY_FLOAT,false);

	assetRigidBody->setCollisionShape(btAssetShape);
}

