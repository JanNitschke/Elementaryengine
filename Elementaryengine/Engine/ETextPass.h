#pragma once
#include "ERenderPass.h"
#include <freetype\freetype.h>
#include <map>
#include <iterator>
#include <iostream>
using namespace std;

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};


class ETextPass :
	public ERenderPass
{
public:
	ETextPass();
	~ETextPass();
	
	string path;
	glm::mat4 projection;

	map<GLchar, Character> Characters;

	virtual void Render();
	virtual void Initialize();

private:
	FT_Library ft;
	FT_Face face;

};

