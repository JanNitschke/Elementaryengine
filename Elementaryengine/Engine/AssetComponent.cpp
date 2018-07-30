#include "AssetComponent.h"
#include <algorithm>
#include <functional>
#include <Asset.h>

AssetComponent::AssetComponent()
{
}


AssetComponent::~AssetComponent()
{
	parent->components.erase(std::remove(parent->components.begin(), parent->components.end(), this), parent->components.end());
	
}

void AssetComponent::AttachTo(Asset * a)
{
	
	parent = a;
	a->components.push_back(this);
}

void AssetComponent::Detach()
{
	if (parent) {
		parent->components.erase(std::remove(parent->components.begin(), parent->components.end(), this), parent->components.end());
	}
}

vec3 AssetComponent::getPosition()
{
	return parent->getPosition();
}
