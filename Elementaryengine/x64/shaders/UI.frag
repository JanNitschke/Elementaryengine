in vec4 FragPos;
layout (location = 0) out vec4 FragCol;

uniform vec3 color;
uniform float opacity;

void main()
{
    FragCol = vec4(color,1) * 1 - opacity + ;
}  