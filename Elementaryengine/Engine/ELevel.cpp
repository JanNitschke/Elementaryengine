#include "ELevel.h"



ELevel::ELevel()
{
}


ELevel::~ELevel()
{
}


void ELevel::Load()
{
	loaded = true;
}

void ELevel::Unload()
{
	loaded = false;
}



void ELevel::setPosition(vec3 p)
{ 
	position = p;
}

vec3 ELevel::getPosition()
{
	return position;
}

void ELevel::setScale(vec3 s)
{
	scale = s;
}

vec3 ELevel::getScale()
{
	return scale;
}

void ELevel::setRotation(quat r)
{
	rotation = r;
}

quat ELevel::getRotation()
{
	return rotation;
}

bool ELevel::isLoaded()
{
	return loaded;
}
