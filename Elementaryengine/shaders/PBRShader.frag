// shadertype=glsl
#version 430 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMaterial;


uniform vec3 directionalLightDirection; 
uniform vec3 directionalLightColor;
uniform vec3 viewPos;
uniform samplerCube depthMaps[20];
uniform float far_plane;
uniform int renderRef; 


layout(std430, binding = 3) buffer lightColors 
{
    vec3 LightColors[];
};
layout(std430, binding = 4) buffer lightPositions 
{
    vec3 LightPositions[];
};
const float PI = 3.14159265359;

vec3 FragPos;

void main(){
	FragPos = texture(gPosition, TexCoord).rgb;
	vec3 norm = texture(gAlbedoSpec, TexCoord).rgb;
	FragColor = vec4(norm,0);  
}
