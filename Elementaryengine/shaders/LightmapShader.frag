in vec4 FragPos;
layout (location = 0) out vec4 FragCol;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);
     
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    //gl_FragDepth = lightDistance;
    gl_FragDepth = lightDistance;
    FragCol = vec4(lightDistance);
}  