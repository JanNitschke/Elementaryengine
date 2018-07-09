layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gMaterial;

layout(std140) uniform Samplers{
	sampler2D Position;
	sampler2D Normal;
	sampler2D AlbedoSpec;
	sampler2D Material;
	sampler2D Depth;
    sampler2D Color;
    sampler2DArray textures;
};

in VertexData
{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
} inData;


uniform vec3 albedo;
uniform float roughness;
uniform vec3 ao;
uniform float metallic;
uniform int albedoTex;
uniform int metallicTex;
uniform int roughnessTex;


void main()
{    
   	vec2 TCoord = inData.TexCoord;

    // store the fragment position vector in the first gbuffer texture
    gPosition = inData.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(inData.Normal);
    // and the diffuse per-fragment color unused a
    gAlbedoSpec.rgb = albedo * texture(textures,vec3(TCoord,albedoTex)).rgb;
    gMaterial.r = roughness * texture(textures,vec3(TCoord,roughnessTex)).r; 
    gMaterial.g = metallic * texture(textures,vec3(TCoord,metallicTex)).r;
    gMaterial.b = ao.r;
}  