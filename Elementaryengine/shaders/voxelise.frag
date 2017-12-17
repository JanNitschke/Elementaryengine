// shadertype=glsl
#version 430 core
out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;   

struct Material {
	sampler2D albedoMap;
	sampler2D roughnessMap;
	sampler2D metallicMap;
	sampler2D normalMap;
	sampler2D aoMap;
	vec3 albedo;
    float roughness;
    vec3 ao;
    float metallic;
}; 
uniform Material material;

layout(std430, binding = 3) buffer lightColors 
{
    vec3 LightColors[];
};
layout(std430, binding = 4) buffer lightPositions 
{
    vec3 LightPositions[];
};
const float PI = 3.14159265359;

void main(){
    FragColor = vec4(texture(material.albedoMap, TexCoord).rgb * material.albedo,1);
}
