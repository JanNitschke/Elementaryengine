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

void AssetComponent::attachTo(Asset * a)
{
	parent = a;
	a->components.push_back(this);
}

void AssetComponent::Render(mat4 view, mat4 projection, Asset* parent)
{
}

void AssetComponent::RenderLightmap(vector<mat4> view, mat4 projection, AssetComponent* l, Asset* a)
{
}

