#version 430 core

out vec4 FragColor;
  
in vec2 TCoord;
in vec3 Normal;  

uniform vec3 directionalLightDirection; 
uniform vec3 directionalLightColor;
uniform mat4 Model;
uniform vec3 viewPos;
uniform vec2 TextureCoordinateMultiply;
uniform samplerCube depthMaps[20];
uniform float far_plane;
uniform samplerCube envMap;
uniform int renderRef; 
uniform mat4 Rot;
uniform vec3 grassBorderColor;
uniform vec3 grassColor;
uniform	sampler2D colorCorrection;

vec3 am = vec3(0.5);
vec3 dif = vec3(0.5);

layout(std430, binding = 3) buffer lightColors 
{
    vec3 LightColors[];
};
layout(std430, binding = 4) buffer lightPositions 
{
    vec3 LightPositions[];
};
const float PI = 3.14159265359;
void main()
{
    vec4 innercolor = vec4(grassColor,0);
    vec4 borderColor = vec4(grassBorderColor,0);
    float b1 = pow(TCoord.x,8);
    float b2 = pow(TCoord.y,8);
    float border = b1 + b2;
    if(border  > 1){
        border = 1;
    }
    vec4 color = border * borderColor + (1-border) * innercolor;

    // Light

	float a_constant = 1.0;
	float a_linear = 0.09;
	float a_quadratic = 0.032;
    float ambientStrength = 0.1;
	float shininess = 2;

	// ambient
    vec3 ambient = (am * (directionalLightColor + vec3(0.1f)));
  	
    // diffuse 
	vec3 normal = mat3(transpose(inverse(Model))) * Normal;  
    float height = pow(TCoord.x * TCoord.y,6);
    normal = normal * (1 - height) + vec3(0,1,0) * height;
    vec3 norm = normalize(normal);
    vec3 norm2 = -norm;
    float diff1 = max(dot(norm, directionalLightDirection), 0.0);
    float diff2 = max(dot(norm2, directionalLightDirection), 0.0);
    float diff = diff1 + diff2;
    vec3 diffuse = diff * directionalLightColor * (dif * color.xyz);

    vec4 outcolor = vec4(ambient + diffuse,0);
    float fragbrightness = (outcolor.x + outcolor.y + outcolor.z) / 3; 


    FragColor = outcolor * vec4(texture(colorCorrection,vec2(0,fragbrightness)));
}