layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

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
layout(std430, binding = 6) buffer didoff 
{
    int offsets[];
};

void main()
{
    int drawid = gl_DrawID + offsets[gl_DrawID] + gl_InstanceID;

    gl_Position = atrib[drawid].Model * vec4(aPos, 1.0);
}  