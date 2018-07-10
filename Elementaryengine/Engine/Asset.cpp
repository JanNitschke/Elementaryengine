#include "Asset.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Game.h"
#include <BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h>
#include <ELevel.h>

const unsigned int Asset::ENVIRONMENT_WIDTH = 1024, Asset::ENVIRONMENT_HEIGHT = 1024;
unsigned int Asset::envMapFBO;
Asset::AssetCallback Asset::rendererAssetCreatedCallback = nullptr;
Asset::AssetCallback Asset::rendererAssetChangedCallback = nullptr;
Asset::AssetCallback Asset::rendererAssetDestroyedCallback = nullptr;

Asset::Asset()
{
	scale = vec3(1.0f);

	OnTick = &defaultOnTick;

	rendererAssetCreatedCallback(this);
	setLevel(Game::mainLevel);
}
Asset::Asset(vec3 pos, vec3 scale, int mass, assetShapes shape)
{
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
	q = glm::quat(vec3(0,0,0));
	btAssetShape->calculateLocalInertia(mass, inertia);
	assetMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(position.x + collisionPosOffset.x, position.y + collisionPosOffset.y, position.z + collisionPosOffset.z)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(mass, assetMotionState, btAssetShape, inertia);
	assetRigidBody = new btRigidBody(groundRigidBodyCI);
	Game::dynamicsWorld->addRigidBody(assetRigidBody);
	assetRigidBody->setFriction(1);
	assetRigidBody->setRestitution(0);
	setLevel(Game::mainLevel);

	rendererAssetCreatedCallback(this);
}

Asset::Asset(vec3 pos, vec3 scale, int mass, assetShapes shape, ELevel * level)
{
	OnTick = &defaultOnTick;

	this->scale = scale;
	position = pos;
	assetShape = shape;
	setLevel(level);

	if (assetShape == assetShapes::ball) {
		btAssetShape = new btSphereShape(scale.x);
	}
	else {
		btAssetShape = new btBoxShape(btVector3(scale.x * collisionSizeOffset.x, scale.y* collisionSizeOffset.y, scale.z* collisionSizeOffset.z));
	}
	btVector3 inertia(1, 1, 1);
	q = glm::quat(vec3(0, 0, 0));
	btAssetShape->calculateLocalInertia(mass, inertia);
	assetMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(getPosition().x + collisionPosOffset.x, getPosition().y + collisionPosOffset.y, getPosition().z + collisionPosOffset.z)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(mass, assetMotionState, btAssetShape, inertia);
	assetRigidBody = new btRigidBody(groundRigidBodyCI);
	Game::dynamicsWorld->addRigidBody(assetRigidBody);
	assetRigidBody->setFriction(1);
	assetRigidBody->setRestitution(0);

	rendererAssetCreatedCallback(this);
}

Asset::~Asset()
{
}


DllExport void Asset::setLevel(ELevel * level)
{
	if (this->level != nullptr) {
		for (auto i = level->assets.begin(); i != level->assets.end(); )
		{
			if (*i._Ptr == this) i = level->assets.erase(i); else ++i;
		}
	}
	this->level = level;
	level->assets.push_back(this);
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

	rendererAssetChangedCallback(this);
}

DllExport void Asset::setPosition(vec3 pos)
{
	position = pos;
	if (assetRigidBody != nullptr) {
		btTransform trans;
		if (vec3(q.x, q.y, q.z).length == 0) {
			trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
		}
		else {
			trans.setRotation(btQuaternion());
		}
		trans.setOrigin(btVector3(pos.x + collisionPosOffset.x, pos.y + collisionPosOffset.y, pos.z + collisionPosOffset.z));
		assetRigidBody->setWorldTransform(trans);
	}
	rendererAssetChangedCallback(this);
}

DllExport void Asset::setFriction(float f)
{
	assetRigidBody->setFriction(f);
}

DllExport void Asset::setRotation(quat rot)
{
	q = rot;
	if (assetRigidBody != nullptr) {
		btTransform trans;
		if (vec3(q.x, q.y, q.z).length == 0) {
			rotation = vec3(0,0,0);
			trans.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
		}
		else {
			trans.setRotation(btQuaternion());
		}
		trans.setOrigin(btVector3(position.x + collisionPosOffset.x, position.y + collisionPosOffset.y, position.z + collisionPosOffset.z));
		assetRigidBody->setWorldTransform(trans);
	}
	rendererAssetChangedCallback(this);
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

DllExport vec3 Asset::getPosition()
{
	return position + level->getPosition();
}

DllExport vec3 Asset::getScale()
{
	return scale * level->getScale();
}

DllExport quat Asset::getRotation()
{
	return rotation * level->getRotation();
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

// called every frame for game logic
void Asset::Tick(GLFWwindow * window, double deltaTime)
{
	if (assetRigidBody != nullptr && Game::simulatePhysics) {
		btTransform trans;
		assetRigidBody->getMotionState()->getWorldTransform(trans);
		if (Game::toGlm(trans.getOrigin()) != position ||
			q.w != trans.getRotation().getW() || q.x != trans.getRotation().getX() || q.y != trans.getRotation().getY() || q.z != trans.getRotation().getZ())
			rendererAssetChangedCallback(this);

		position.x = trans.getOrigin().getX() - collisionPosOffset.x;
		position.y = trans.getOrigin().getY() - collisionPosOffset.y;
		position.z = trans.getOrigin().getZ() - collisionPosOffset.z;

		q.w = trans.getRotation().getW();
		q.x = trans.getRotation().getX();
		q.y = trans.getRotation().getY();
		q.z = trans.getRotation().getZ();

	}
	OnTick(window, deltaTime, this);
}

DllExport void Asset::setTickFunction(void(*tickFunction)(GLFWwindow *window, double deltaTime, Asset* asset))
{
	OnTick = tickFunction;
}

void Asset::SetupAsset()
{

}

void Asset::RenderGeometry()
{
}

void Asset::Destroy()
{
	Game::assetsToDelete.push_back(this);
	for (int i = 0; i < components.size(); i++)
	{
		delete components[i];
	}
	components.clear();
	if (assetRigidBody) {
		Game::dynamicsWorld->removeRigidBody(assetRigidBody);
		delete assetRigidBody;

	}
}

void Asset::setHeightmapCollision(const char * path)
{
	// TODO: redo for new Renderer
	/*int width, height, nrChannels;
	unsigned char* data = Texture::LoadPicture(path);
	btAssetShape = new btHeightfieldTerrainShape(512, 512, data, scale.y, 2, false, false);
	btAssetShape = new btHeightfieldTerrainShape(512, 512, data,1,0,1,1,PHY_ScalarType::PHY_FLOAT,false);

	assetRigidBody->setCollisionShape(btAssetShape);*/
}

