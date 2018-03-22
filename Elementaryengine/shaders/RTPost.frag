#version 430 core
in vec2 TexCoord;
layout (location = 0) out vec4 FragColor;

uniform sampler2D rtResult;

void main(){
	vec3 color = texture(rtResult, TexCoord).rgb;
	FragColor = vec4(color,0);
}