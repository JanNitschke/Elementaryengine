layout (location = 0) out vec4 FragCol;

in VertexData
{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
} inData;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(inData.FragPos - lightPos);
     
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    //gl_FragDepth = lightDistance;
    gl_FragDepth = lightDistance;
    FragCol = vec4(lightDistance);
}  