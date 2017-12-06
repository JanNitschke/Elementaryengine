#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 Model;
out vec2 TCoord;
out vec3 Norm;

void main()
{
	TCoord = aTexCoord;    
	Norm = aNormal;  
    gl_Position = Model * vec4(aPos, 1.0);
}  