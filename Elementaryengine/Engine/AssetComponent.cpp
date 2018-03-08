#include "AssetComponent.h"
#include <algorithm>
#include <functional>
#include <Asset.h>

AssetComponent::AssetComponent()
{
}


AssetComponent::~AssetComponent()
{
	for each (auto p in parents)
	{
		p->components.erase(std::remove(p->components.begin(), p->components.end(), this), p->components.end());
	}
}

void AssetComponent::attachTo(Asset * a)
{
	parents.push_back(a);
	a->components.push_back(this);
}

void AssetComponent::detachFrom(Asset * a)
{
	parents.erase(std::remove(parents.begin(), parents.end(), a), parents.end());
}
