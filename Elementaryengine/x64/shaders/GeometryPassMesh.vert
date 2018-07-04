layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VertexData
{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
} outData;

uniform mat4 VP;
uniform mat4 Model;
uniform mat4 Rot;
uniform vec3 albedo;



void main()
{
    outData.TexCoord = aTexCoord; 
	outData.FragPos = vec3(Model * Rot * vec4(aPos, 1.0));
    gl_Position = VP * Model * Rot * vec4(aPos, 1.0);
	outData.Normal = vec3(Model * Rot * vec4(aNormal,0));  
}