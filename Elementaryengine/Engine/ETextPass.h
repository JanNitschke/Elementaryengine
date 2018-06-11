#pragma once
#include "ERenderPass.h"
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <map>
#include <iterator>
#include <iostream>
#include <EOGLUniform.h>

using namespace std;

struct Character {
	GLuint     textureID;  // ID handle of the glyph texture
	glm::ivec2 size;       // Size of glyph
	glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
	GLuint     advance;    // Offset to advance to next glyph
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
	GLuint VAO, VBO;
	EOGLUniform<mat4> proj;
	EOGLUniform<vec3> textColor;
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
};

