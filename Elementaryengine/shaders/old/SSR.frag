// shadertype=glsl
#version 430 core
layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMaterial;
uniform sampler2D gDepth;
uniform sampler2D gColor;
uniform mat4 invView;
uniform mat4 invProj;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 viewPos;

const float rayStep = 0.02;
const float minRayStep = 0.1;
const int maxSteps = 60;
const float searchDist = 5;
const int numBinarySearchSteps = 5;
const float refSpecFallofExp = 3.0;

float getDepth(vec2 pos){
    return length(texture(gPosition,TexCoord).rgb - viewPos);
}
vec4 RayCast(in vec3 dir, inout vec3 hitCoord, out float dDepth){
    dir += rayStep;

    float depth = 0.0;
    int steps = 0;
    vec4 projectedCoord = vec4(0.0);

    for(int i = 0; i < maxSteps; ++i){
        hitCoord += dir;
        projectedCoord = proj * view * vec4(hitCoord,1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = texture(gDepth, projectedCoord.xy).z * 25;
        if(depth > 1000.0)
            continue;

            dDepth = hitCoord.z - depth;

            if((dir.z - dDepth) < 1.2)
            if(dDepth <= 0){
                vec4 Result = vec4(projectedCoord.xy,dDepth,1.0);
                return Result;
            }
    }
    return vec4(-1);
}

void main(){
	float roughness = texture(gMaterial, TexCoord).r;
    float metallic = texture(gMaterial, TexCoord).g;
    vec3 viewNormal = vec3(texture(gNormal, TexCoord));
    vec3 globalPos =  vec3(texture(gPosition, TexCoord));
    vec3 Depth = vec3(texture(gDepth, TexCoord));
    vec3 vPos = globalPos - viewPos;
    vec3 reflected = vec4(normalize(reflect(normalize(vPos),normalize(viewNormal))),1).xyz;

    vec3 hitPos = globalPos;
    float dDepth;
    
    if(roughness < 0.2 && metallic > 0.5){
        vec2 pos = RayCast(reflected,hitPos,dDepth).xy;
        if(pos != vec2(-1)){
            FragColor = roughness * vec4(texture(gColor,TexCoord).rgb,1) + (1 - roughness) * vec4(texture(gColor,RayCast(reflected,hitPos,dDepth).xy).rgb,1) ;
        }else{
            FragColor = vec4(texture(gColor,TexCoord).rgb,1);
        }
    }else{
        FragColor = vec4(texture(gColor,TexCoord).rgb,1);
    }
    //FragColor = vec4(texture(gColor,TexCoord).rgb,1);

    //FragColor = vec4(reflected,1);
}
