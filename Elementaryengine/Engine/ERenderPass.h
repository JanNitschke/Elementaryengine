#pragma once
#include <Shader.h>
#include <EOGLUniform.h>
#include <memory>
#include <EOpenGL.h>
class ERenderPass
{
protected:
	vector<EOGLBaseUniform*> _uniforms;
	vector<GLuint> _inputTextures;
	vector<GLuint> _outputTextures;
	Shader* _shader;

public:
	ERenderPass();
	~ERenderPass();
	virtual void Render();
	virtual void Initialize();

	GLuint renderBuffer;
	EDisplaySettings* displaySettings;

};

