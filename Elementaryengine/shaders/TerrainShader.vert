#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TeCoord;
out vec3 FrPos;
out vec3 Norm;
out vec2 localPos;

uniform mat4 Model;
uniform mat4 Rot;
uniform mat4 VP;
uniform sampler2D heightmap;

float height(vec2 Coords){
   return texture(heightmap,Coords).r;
}

void main()
{
    TeCoord = aTexCoord; 
	FrPos = vec3(Model *  Rot * vec4(aPos + vec3(0,height(aTexCoord),0), 1.0));
    float grid = 0.001953125;
    vec2 c1 = aTexCoord + vec2(0,grid);
    vec2 c2 = aTexCoord + vec2(grid,0);
    vec2 c3 = aTexCoord + vec2(0, 0 - grid);
    vec2 c4 = aTexCoord + vec2(0 - grid,0);

    vec3 V0 = vec3(Model * vec4(0,height(aTexCoord),0,0));
    vec3 V1 = vec3(Model * vec4(0,height(c1),grid,0));
    vec3 V2 = vec3(Model * vec4(grid,height(c2),0,0));
    vec3 V3 = vec3(Model * vec4(0,height(c3),-grid,0));
    vec3 V4 = vec3(Model * vec4(-grid,height(c4),0,0));

    vec3 N1 = normalize(cross(V1 - V0, V2 - V0));
    vec3 N2 = normalize(cross(V2 - V0, V3 - V0));
    vec3 N3 = normalize(cross(V3 - V0, V4 - V0));
    vec3 N4 = normalize(cross(V4 - V0, V1 - V0));
	Norm = normalize(N1 + N2 + N3 + N4);  
    localPos = aPos.xz;
    gl_Position = Model * Rot * vec4(aPos + vec3(0,height(aTexCoord),0), 1.0);
}