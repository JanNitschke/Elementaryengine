#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=512) out;

in vec2 TeCoord[3];
in vec3 Norm[3];
in vec3 FrPos[3];
in vec2 localPos[3];
out vec2 TCoord;
out vec3 Normal;

uniform mat4 VP;
uniform float timeRunning;
uniform sampler2D grassMap;
uniform float rndoffset;
uniform mat4 Model;

uniform sampler2D heightmap;

float height(vec2 Coords){
   return texture(heightmap,Coords).r;
}

void main()
{
    mat4 invModel = inverse(Model);
    float maxHeight = 1.2;
    float width = 0.02;
    vec4 offsetDown = vec4(0,-0.1,0,0);
    float maxDist = length(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz);
    for(int vert = 0; vert < 3; ++vert)
    {
        if(texture(grassMap, TeCoord[vert]).r > 0.1){
            float height = maxHeight * texture(grassMap, TeCoord[vert]).r; 
            vec4 verPos = gl_in[vert].gl_Position;
            vec3 fpos = FrPos[vert];
            float srnd = 2.34;
            float rnd = 2.34 + rndoffset;
            for(int i = 0; i < 12; i++){
                rnd = rnd * rnd;
                if (rnd > 600000 ){
                    srnd += 0.12;
                    rnd = srnd;
                }
                float r1 = sin(fpos.x * (1528.3 + rnd)) + cos(fpos.x * 75.9);
                float r2 = cos(fpos.y * 537.8 + rndoffset) + sin(fpos.y * (1753+rnd));
                float r3 = sin(fpos.x * (758.3+rnd)) + cos(fpos.y * 43.2);
                float r4 = cos(fpos.y * 537.8 + rndoffset) + sin(fpos.x * (1475.4+rnd));

                vec4 windOffset = vec4(sin(timeRunning + fpos.x), 0 , sin(timeRunning + fpos.y),0) * 0.1;

                vec3 pOffset = vec3(r2,0,r4) * maxDist / 1;
                vec3 direction = vec3(r3,0,r1) * maxDist / 2;
                
                vec2 loff = localPos[vert] + vec2(pOffset.x, pOffset.z);
                float hdif = texture(heightmap,localPos[vert]).r - texture(heightmap,loff).r;
                vec4 offsetDown = 0.5 * vec4(0, hdif ,0,0);
 
                //vec4 V[3] = vec4[3](vec4(0),vec4(0),vec4(0));
                vec4 V0 = verPos + vec4(pOffset,0) + offsetDown;
                vec4 V1 = verPos + vec4(pOffset,0) + vec4(width / 2 * normalize(direction),0) + vec4(0,height,0,0) + windOffset + offsetDown;
                vec4 V2 = verPos + vec4(pOffset,0) + vec4(width * normalize(direction),0) + offsetDown;

        
            //  vec4 F[3]= vec4[3](vec4(0),vec4(0),vec4(0));
                vec4 F0 = vec4(fpos,0) + vec4(pOffset,0)  + offsetDown;
                vec4 F1 = vec4(fpos,0) + vec4(pOffset,0) + vec4(width / 2 * normalize(direction),0) + vec4(0,height,0,0) + windOffset;
                vec4 F2 = vec4(fpos,0) + vec4(pOffset,0) + vec4(width * normalize(direction),0) + offsetDown;
                vec3 A = V2.xyz - V0.xyz;
                vec3 B = V1.xyz - V0.xyz;
                vec4 normal = vec4(normalize(cross(A,B)),0);

            // vec2 Tex[3];
                vec2 Tex0 = vec2(1,0);
                vec2  Tex1 = vec2(1,1);
                vec2  Tex2 = vec2(0,1);

                TCoord = Tex0; 
                Normal = normal.xyz;
                gl_Position = VP * V0;
                EmitVertex();

                TCoord = Tex1; 
                Normal = normal.xyz;
                gl_Position = VP * V1;
                EmitVertex();

                TCoord = Tex2; 
                Normal = normal.xyz;
                gl_Position = VP * V2;
                EmitVertex();

                EndPrimitive();
            }
        }
    }
}  