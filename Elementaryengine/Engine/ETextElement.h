#pragma once
#include <string>
#include <glm\glm.hpp>

using namespace std;
using namespace glm;

struct ETextElement
{
public:
	string text;
	float posX, posY, scale;
	vec3 color;
	ETextElement();
	ETextElement(string Text, float PositionX, float PositionY, float Scale, vec3 Color) {
		text = Text;
		posX = PositionX;
		posY = PositionY;
		scale = Scale;
		color = Color;
	}
};