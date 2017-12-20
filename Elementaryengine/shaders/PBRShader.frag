// shadertype=glsl
#version 430 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMaterial;
uniform samplerCubeArray shadowMaps;
uniform	sampler2D colorCorrection;

uniform vec3 directionalLightDirection; 
uniform vec3 directionalLightColor;
uniform vec3 viewPos;
uniform float far_plane;


layout(std430, binding = 3) buffer lightColors 
{
    vec3 LightColors[];
};
layout(std430, binding = 4) buffer lightPositions 
{
    vec3 LightPositions[];
};
const float PI = 3.14159265359;

vec3 sampleOffsetDirections[20] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);  
float ShadowCalculation(vec3 fragPos, vec3 lightPos, int index)
{
	float bias   = 0.10;
	int samples  = 20;
	float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 100.0;

	// get vector between fragment position and light position
	vec3 FPos = fragPos;
	vec3 l = lightPos;
	vec3 fragToLight = FPos - l;


    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
	
	float shadow = 0.0;
	for(int i = 0; i < samples; ++i)
	{
		float cDepth = texture(shadowMaps, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius,index)).r;
		cDepth *= far_plane;   // Undo mapping [0;1]
		if( currentDepth - bias > cDepth)
		shadow += 1.0;
	}
	
	shadow /= float(samples);  

    return shadow;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  
void main(){
	vec3 FragPos = texture(gPosition, TexCoord).rgb;
	vec3 Normal = texture(gNormal, TexCoord).rgb;
	vec3 albedo = pow(texture(gAlbedoSpec, TexCoord).rgb, vec3(0.8));
	float roughness = texture(gMaterial, TexCoord).r;
	float metallic = texture(gMaterial, TexCoord).g;
	float ambient = texture(gMaterial, TexCoord).b;
	vec3 outcolor;

	vec3 N = normalize(Normal); 
    vec3 V = normalize(viewPos - FragPos);
	vec3 Lo = vec3(0.0);

	vec3 dL = normalize(directionalLightDirection);
	vec3 dH = normalize(V + dL);

	vec3 dradiance = directionalLightColor * 0.08; 

	vec3 dF0 = vec3(0.04); 
	dF0 = mix(dF0, albedo, metallic);
	vec3 dF  = fresnelSchlick(max(dot(dH, V), 0.0), dF0);
	float dNDF = DistributionGGX(N, dH, roughness);       
	float dG = GeometrySmith(N, V, dL, roughness); 
	vec3 dnominator    = dNDF * dG * dF;
	float ddenominator = 4 * max(dot(N, V), 0.0) * max(dot(N, dL), 0.0) + 0.001; 
	vec3 dspecular     = dnominator / ddenominator * 10;     

	//specular and refraction components  
	vec3 dkS = dF;
	vec3 dkD = vec3(1.0) - dkS;
	dkD *= 1.0 - metallic;	
	float dNdotL = max(dot(N, dL), 0.0);        
	Lo += (dkD * albedo / PI + dspecular) * dradiance * dNdotL;


	//for each light ... add to lo
	for(int i=0;i<int(LightColors.length());i++){
	    vec3 L = normalize(LightPositions[i] - FragPos);
		vec3 H = normalize(V + L);
	
		float distance = length(LightPositions[i] - FragPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = LightColors[i] * attenuation; 

		vec3 F0 = vec3(0.04); 
		F0 = mix(F0, albedo, metallic);
		vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

		float NDF = DistributionGGX(N, H, roughness);       
		float G = GeometrySmith(N, V, L, roughness); 
		vec3 nominator    = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
		vec3 specular     = nominator / denominator;     

		//specular and refraction components  
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;	

		float shadow = ShadowCalculation(FragPos,LightPositions[i],i);        
		float NdotL = max(dot(N, L), 0.0);        
		Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1 - shadow);
	}   
	vec3 am = vec3(0.3) * albedo * ambient;
	outcolor   = am + Lo;  
	float fragbrightness = (outcolor.x + outcolor.y + outcolor.z) / 3; 	
	vec4 colCor = vec4(texture(colorCorrection,vec2(0,fragbrightness)));
	FragColor = vec4(outcolor, 1.0) * colCor * colCor * 5;
}
