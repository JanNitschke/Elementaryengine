// shadertype=glsl
#version 430 core

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;   
in vec3 localPos;

uniform vec3 directionalLightDirection; 
uniform vec3 directionalLightColor;
uniform mat4 Model;
uniform vec3 viewPos;
uniform vec2 TextureCoordinateMultiply;
uniform samplerCube depthMaps[20];
uniform float far_plane;
uniform samplerCube envMap;
uniform int renderRef; 

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
float ShadowCalculation(vec3 fragPos, vec3 lightPos, int light)
{
	float shadow = 0.0;
	float bias   = 0.10;
	int samples  = 20;
	float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 100.0;

	// get vector between fragment position and light position
	vec3 FPos = FragPos;
	vec3 l = lightPos;
	vec3 fragToLight = FPos - l;

    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(depthMaps[light], fragToLight).r;

    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;

    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(depthMaps[light], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;   // Undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);  
    return shadow;;
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
void main()
{
	vec2 TCoord = TexCoord * TextureCoordinateMultiply;
	vec3 outcolor;
	vec3 N = normalize(Normal); 
    vec3 V = normalize(viewPos - FragPos);
	vec3 Lo = vec3(0.0);
	// normal map

	// obtain normal from normal map in range [0,1]
    vec3 normal = texture(material.normalMap, TexCoord).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0); 


	//get the pbr values of the pixel
	//vec3 albedo = material.albedo * vec3(texture(material.albedoMap, TCoord));
	vec3 albedo = pow(texture(material.albedoMap, TCoord).rgb * material.albedo , vec3(0.8));
	float metallic = material.metallic * vec3(texture(material.metallicMap, TCoord)).r;
	float roughness = material.roughness * vec3(texture(material.roughnessMap, TCoord)).r;

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
		Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1- shadow);
	}   
	vec3 prefilteredColor = vec3(0.0);     
	roughness += 0.2;
	if(renderRef == 1){
		if(roughness < 0.5){
			const uint SAMPLE_COUNT = 512u;
			float totalWeight = 0.0;   
			for(uint i = 0u; i < SAMPLE_COUNT; ++i)
			{
				vec2 Xi = Hammersley(i, SAMPLE_COUNT);
				vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
				vec3 L  = normalize(2.0 * dot(V, H) * H - V);

				float NdotL = max(dot(N, L), 0.0);
				if(NdotL > 0.0)
				{
					prefilteredColor += texture(envMap, L).rgb * NdotL;
					totalWeight      += NdotL;
				}
			}
			prefilteredColor = prefilteredColor / totalWeight;
		} 
	}
 
	vec3 ambient = vec3(0.03) * albedo * material.ao;
	outcolor   = ambient + Lo;  
	FragColor = vec4(outcolor, 1.0) + (vec4(prefilteredColor,0) *0.6 * (1 - roughness) *  (metallic + 0.2));
}
