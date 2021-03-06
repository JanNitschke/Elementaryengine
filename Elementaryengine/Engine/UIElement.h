#pragma once
#include <glm\glm.hpp>
#include <Texture.h>

using namespace glm;
class UIElement
{
public:

	Texture* texture;
	Texture* alphamap;

	vec2 positionPixel;
	vec2 posisionPercent;
	vec2 sizePixel;
	vec2 sizePercent;

	vec3 foregroundColor;
	vec3 backgroundColor;

	float backgoundBlur;
	float foregroundBlur;
	float opacity;
	float zindex;
	UIElement();
	~UIElement();
};