#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 envMatrices[6];

in vec2 TCoord[3];
in vec3 Norm[3];

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position.xyz;
            vec4 FPos = gl_in[i].gl_Position;
			TexCoord = TCoord[i];
			Normal = Norm[i];
            gl_Position = envMatrices[face] * FPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  