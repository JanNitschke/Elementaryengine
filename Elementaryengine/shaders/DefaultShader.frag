// shadertype=glsl
#version 430 core

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;   

uniform vec3 directionalLightDirection; 
uniform vec3 directionalLightColor;
uniform mat4 Model;
uniform vec3 viewPos;
uniform vec2 TextureCoordinateMultiply;
uniform samplerCube depthMaps[20];
uniform float far_plane;

struct Material {
	sampler2D Texture;
	sampler2D specularMap;
	sampler2D ambientMap;
	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
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

void main()
{
	vec2 TCoord = TexCoord * TextureCoordinateMultiply;
	float a_constant = 1.0;
	float a_linear = 0.09;
	float a_quadratic = 0.032;
    float ambientStrength = 0.1;
	float shininess = 32;

	// ambient
    vec3 ambient = (material.ambient * (directionalLightColor + vec3(0.1f)));
  	
    // diffuse 
	vec3 normal = mat3(transpose(inverse(Model))) * Normal;  

    vec3 norm = normalize(normal);
    float diff = max(dot(norm, directionalLightDirection), 0.0);
    vec3 diffuse = diff * directionalLightColor * (material.diffuse * vec3(texture(material.Texture, TCoord)));

	// specular
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(directionalLightDirection + viewDir);

	vec3 reflectDir = reflect(-directionalLightDirection, norm);  
	float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
	vec3 specular = (material.specular * vec3(texture(material.specularMap, TCoord))) * spec * directionalLightColor;  


	//add diffuse of each light
	vec3 pDiffuse = vec3(0);
	vec3 pSpecular = vec3(0);
	vec3 outcolor;
	for(int i=0;i<int(LightColors.length());i++){
		float distance    = length(LightPositions[i] - FragPos);
		float attenuation = 1.0 / (a_constant + a_linear * distance + a_quadratic * (distance * distance));    

		vec3 pnorm = normalize(normal);
		vec3 plightDir = normalize(LightPositions[i] - FragPos);  

		float shadow = ShadowCalculation(FragPos,LightPositions[i],i);                      

		//Diffuse
		float pdiff = max(dot(pnorm, plightDir), 0.0);
		pDiffuse =  pDiffuse + (pdiff * LightColors[i] * (material.diffuse * vec3(texture(material.Texture, TCoord))) * attenuation * (1.0 - shadow));

		// specular
		vec3 pReflectDir = reflect(-plightDir, norm);  
		vec3 halfwayDir = normalize(plightDir + viewDir);
		float pSpec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess); 

		pSpecular = pSpecular + ((material.specular * vec3(texture(material.specularMap, TCoord))) * pSpec * LightColors[i] * attenuation * (1.0 - shadow));  
	}   

	
    vec3 directional = (ambient + diffuse + pDiffuse + specular + pSpecular);

	FragColor = vec4(directional, 1.0);
}
