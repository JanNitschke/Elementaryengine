#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TCoord;
out vec3 Normal;

uniform mat4 Model;
uniform mat4 Rot;

void main()
{
    TCoord = aTexCoord; 
    gl_Position = Model * Rot *vec4(aPos, 1.0);
	Normal = vec3(Model * Rot * vec4(aNormal,0));  
}