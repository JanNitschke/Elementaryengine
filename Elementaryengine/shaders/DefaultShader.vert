#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in uint drawid;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out flat uint did;
uniform mat4 VP;

struct DrawAtributes{
    mat4 Model;
	mat4 Rot;
	vec3 albedo;
	vec3 ao;
	float roughness;
	float metallic;
};
layout(std430, binding = 5) buffer Atrib 
{
    DrawAtributes atrib[];
};

void main()
{
    TexCoord = aTexCoord; 
	FragPos = vec3(atrib[drawid].Model * atrib[drawid].Rot * vec4(aPos, 1.0));
    gl_Position = VP * atrib[drawid].Model * atrib[drawid].Rot * vec4(aPos, 1.0);
	Normal = vec3(atrib[drawid].Model * atrib[drawid].Rot * vec4(aNormal,0));  
    did = drawid;
}