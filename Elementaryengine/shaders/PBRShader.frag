layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

layout(std430, binding = 15) buffer lightColors 
{
    vec3 LightColors[];
};
layout(std430, binding = 16) buffer lightPositions 
{
    vec3 LightPositions[];
};

layout(std140, binding = 1) uniform Samplers{
	sampler2D gPosition;
	sampler2D gNormal;
	sampler2D gAlbedoSpec;
	sampler2D gMaterial;
	sampler2D gDepth;
    sampler2D gColor;
	samplerCubeArrayShadow shadowMaps;
    sampler2DArray textures;
};



uniform	sampler2D colorCorrection;

uniform vec3 directionalLightDirection; 
uniform vec3 directionalLightColor;
uniform vec3 viewPos;
uniform float far_plane;
uniform mat4 invProj;
uniform mat4 invView;

const float PI = 3.14159265359;

float ShadowCalculation(vec3 fragPos, int index)
{
	vec3 lightPos = LightPositions[index];
	float bias = 0.15;

	// get vector between fragment position and light position
	vec3 fragToLight = fragPos - lightPos;

    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
	

	float cDepth = texture(shadowMaps, vec4(fragToLight , index),(currentDepth - bias)	/ far_plane).r;

    return cDepth;
}

float lightVolume(int index,float depth){
	vec3 lightPos = LightPositions[index];
	int sampleCount = vlSampleCount;
	float bias = 0.1;
	if(depth > 5){
		depth = 5;
	}
	depth = (depth * (far - near)) - near;
	float strength = 0;
	float dX = (2 * TexCoord.x ) - 1;
	float dY = (2 * TexCoord.y ) - 1; 
	vec4 dir = normalize(invView * vec4((invProj * vec4(dX,dY,0.1,1)).xyz,0));
	float stepLength = depth / sampleCount;
	stepLength /= 2;
	vec4 currentPosition = vec4(viewPos,1);
	float le = 0;
	float dfp = 0;
	for(int i = 0; i < sampleCount; i++){
		currentPosition += (dir * stepLength);
		vec3 posToLight = currentPosition.xyz - lightPos;
		float currentDepth = length(currentPosition.xyz - viewPos);
		float currentDepthToLight = length(posToLight);
		dfp += stepLength;
		float attenuation = 1.0 / (currentDepthToLight) * (currentDepthToLight);
		if(attenuation > 0.2){
			float cDepth = texture(shadowMaps, vec4(posToLight, index),(currentDepthToLight - bias)	/ far_plane).r;
			strength += (-1 + (2 * cDepth * cDepth)) * attenuation;
			//strength += ((cDepth / dfp) + cDepth) * attenuation;
		}

	}
	return strength * depth / sampleCount;

	if(depth / sampleCount > 0.97){
		return 0.0;
	}else{
		if(depth / sampleCount > vlMax){
			return strength * vlMax;
		}else{
			return strength * depth / sampleCount;
		}
	}
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
	float FDepth = texture(gDepth, TexCoord).r;
	vec4 DDepth = texture(gDepth, TexCoord).rgba;
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
	float liniarDepth = (2.0 * near) / (near + far - DDepth.r * (far - near));


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

		float shadow = ShadowCalculation(FragPos,i);      
		float NdotL = max(dot(N, L), 0.0);  
		float lr = 0;
		vec3 LoAdd = (kD * albedo / PI + specular) * radiance * NdotL * shadow;
		if(useBasicVl){
			lr = lightVolume(i,liniarDepth);
			LoAdd *= 3;
			lr *= 0.1;
			lr += 0.5;
			if(lr > 2) lr = 2;
			LoAdd *=lr; 
		}else{
			LoAdd *= 4;
		}
		Lo += LoAdd;

	}   
	vec3 am = vec3(0.3) * albedo * ambient;

	outcolor   = am + Lo;  
	FragColor = vec4(outcolor * 5, 1.0);
	//FragColor = vec4(liniarDepth * 0.05);
}
