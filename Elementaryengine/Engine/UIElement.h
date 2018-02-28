#pragma once
#include <glm\glm.hpp>
#include <Texture.h>

using namespace glm;
class UIElement
{
public:
	vec2 position;
	vec2 positionPercent;
	vec2 size;
	vec2 SizePercent;
	vec3 color;
	Texture* texture;
	UIElement();
	~UIElement();
};