#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec2 TeCoord[3];
in vec3 Norm[3];
in vec3 FrPos[3];
in vec3 localPos[3];

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 VP;

void main()
{
    vec3 A = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    vec3 B = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
    vec3 normal = normalize(cross(A,B));
    for(int i = 0; i < 3; ++i){
        FragPos = FrPos[i];
        TexCoord = TeCoord[i]; 
        Normal = Norm[i];
        gl_Position = VP * gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}  