// shadertype=glsl
#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gMaterial;

in vec2 TexCoord;
in vec3 FragPos;  
in vec3 Normal;  
in flat uint did;
 
in flat uint drawid;

uniform vec2 TextureCoordinateMultiply;

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
   	vec2 TCoord = TexCoord * TextureCoordinateMultiply;

    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color unused a
    gAlbedoSpec.rgb = atrib[did].albedo;
    gMaterial.r = atrib[did].roughness;
    gMaterial.g = atrib[did].metallic;
    gMaterial.b = atrib[did].ao.r;
}  