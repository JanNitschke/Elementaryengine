#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec3 localPos;

uniform mat4 Model;
uniform mat4 VP;

void main()
{
    TexCoord = aTexCoord; 
    localPos = aPos;   
	FragPos = vec3(Model * vec4(aPos, 1.0));
    gl_Position = VP * Model * vec4(aPos, 1.0);
	Normal = aNormal;  
}