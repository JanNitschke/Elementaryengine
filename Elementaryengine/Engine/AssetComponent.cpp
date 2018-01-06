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

void AssetComponent::Render(mat4 view, mat4 projection, Asset* parent, Shader* s)
{
}

void AssetComponent::RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a)
{
}

