#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

in vec2 TCoord[3];
in vec3 Norm[3];
in vec3 localPos[3];
out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
    for(int vert = 0; vert < 3; ++vert)
    {
        for(int triang = 0; triang < 2; ++triang){
            int n;
            if(triang == 0){
                n = vert;
            }
            if(triang == 1){
                if(vert < 2){
                    n = vert + 1;
                }else{
                    n = 0;
                }
            }
            FragPos = gl_in[n].gl_Position.xyz;
            vec4 FPos = gl_in[n].gl_Position;
            Normal = Norm[n];
            if(triang == 1){
                TexCoord = vec2(1,0);
                gl_Position = FPos;
            }else{
                TexCoord = vec2(0,1);
                gl_Position = FPos + vec4(0,1,0,0);
            }   
            EmitVertex();
            for(int i = 0; i < 3; ++i){
                if(i == vert || i == vert + 1 || i == 0 && vert == 2){
                    FragPos = gl_in[i].gl_Position.xyz;
                    vec4 FPos = gl_in[i].gl_Position;
                    TexCoord = TCoord[i];
                    Normal = Norm[i];
                    
                    if(triang == 0){
                        if(i == vert){
                            TexCoord = vec2(0,0);
                        }else{
                            TexCoord = vec2(1,0);
                        }
                        gl_Position = FPos;
                    }else{
                        if(i == vert){
                            TexCoord = vec2(0,1);
                        }else{
                            TexCoord = vec2(1,1);
                        }
                        gl_Position = FPos + vec4(0,1,0,0);
                    }                
                    EmitVertex();
                }  
            }
        }
        EndPrimitive();
    
    }
}  