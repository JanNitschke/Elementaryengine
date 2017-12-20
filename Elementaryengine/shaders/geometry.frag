// shadertype=glsl
#version 460 core
#extension GL_ARB_bindless_texture : enable
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gMaterial;

in vec2 TexCoord;
in vec3 FragPos;  
in vec3 Normal;  
in flat uint did;
 
uniform sampler2DArray textures;

struct DrawAtributes{
    mat4 Model;
	mat4 Rot;
	vec3 albedo;
    float roughness;
	vec3 ao;
	float metallic;
    int albedoTex;
	int metallicTex;
	int roughnessTex;
};
layout(std430, binding = 5) buffer Atrib 
{
    DrawAtributes atrib[];
};

void main()
{    
   	vec2 TCoord = TexCoord;

    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color unused a
    gAlbedoSpec.rgb = atrib[did].albedo * texture(textures,vec3(TCoord,atrib[did].albedoTex)).rgb;
    gMaterial.r = atrib[did].roughness * texture(textures,vec3(TCoord,atrib[did].roughnessTex)).r; 
    gMaterial.g = atrib[did].metallic * texture(textures,vec3(TCoord,atrib[did].metallicTex)).r;
    gMaterial.b = atrib[did].ao.r;
}  