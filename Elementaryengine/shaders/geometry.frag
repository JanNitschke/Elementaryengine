// shadertype=glsl
#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gMaterial;
 
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;   

uniform mat4 Model;
uniform vec3 viewPos;
uniform vec2 TextureCoordinateMultiply;
uniform samplerCube depthMaps[20];
uniform float far_plane;
uniform samplerCube envMap;
uniform int renderRef; 
uniform mat4 Rot;

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

void main()
{    
   	vec2 TCoord = TexCoord * TextureCoordinateMultiply;

    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color unused a
    gAlbedoSpec.rgb = texture(material.albedoMap, TCoord).rgb * material.albedo;
    gMaterial.r = material.roughness * vec3(texture(material.roughnessMap, TCoord)).r;
    gMaterial.g = material.metallic * vec3(texture(material.metallicMap, TCoord)).r;
    gMaterial.b = material.ao.r * vec3(texture(material.aoMap, TCoord)).r;
}  