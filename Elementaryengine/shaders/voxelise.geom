#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 voxelprojection;
uniform vec3 viewPos;

in vec2 TCoord[3];
in vec3 Norm[3];

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
    float z = 0;
    for(int i = 0; i < 3; ++i) // for each triangle's vertices
    {
        z += gl_in[i].gl_Position.z;
    }
    z = z / 3;
    for(int i = 0; i < 3; ++i) // for each triangle's vertices
    {
        FragPos = gl_in[i].gl_Position.xyz;
        vec4 FPos = gl_in[i].gl_Position;
        TexCoord = TCoord[i];
        Normal = Norm[i];
        gl_Position = voxelprojection * FPos;
        gl_Layer = int(round(z));
        EmitVertex();
    }    
    EndPrimitive();
}  