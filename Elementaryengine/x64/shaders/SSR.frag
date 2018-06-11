// shadertype=glsl
layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;


struct UIElement {
	vec2 positionPixel;
	vec2 posisionPercent;
	vec2 sizePixel;
	vec2 sizePercent;
	vec3 foregroundColor;
    int texture;
	vec3 backgroundColor;
    int alphamap;
	float backgoundBlurr;
	float foregroundBlurr;
	float opacity;
    float z;
};

layout(std430, binding = 7) buffer UIE 
{
    UIElement uie[];
};

uniform sampler2DArray textures;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMaterial;
uniform sampler2DShadow gDepth;
//uniform sampler2D gDepth;
uniform sampler2D gColor;
uniform mat4 invView;
uniform mat4 invProj;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 viewPos;
uniform int screenX;
uniform int screenY;

const float rayStep = 0.1;
const float minRayStep = 0.1;
const int maxSteps = 60;
const float searchDist = 5;
const int numBinarySearchSteps = 5;
const float refSpecFallofExp = 3.0;

const float weights[] = {
        
    0.023528,	0.033969,	0.038393,	0.033969,	0.023528,
    0.033969,	0.049045,	0.055432,	0.049045,	0.033969,
    0.038393,	0.055432,	0.062651,	0.055432,	0.038393,
    0.033969,	0.049045,	0.055432,	0.049045,	0.033969,
    0.023528,	0.033969,	0.038393,	0.033969,	0.023528
};

//http://dev.theomader.com/gaussian-kernel-calculator/
const float hqweights[] = {
    
    0.000086,	0.00026,	0.000614,	0.001132,	0.001634,	0.001847,	0.001634,	0.001132,	0.000614,	0.00026,	0.000086,
    0.00026,	0.000784,	0.001848,	0.003408,	0.00492,	0.005561,	0.00492,	0.003408,	0.001848,	0.000784,	0.00026,
    0.000614,	0.001848,	0.004354,	0.00803,	0.011594,	0.013104,	0.011594,	0.00803,	0.004354,	0.001848,	0.000614,
    0.001132,	0.003408,	0.00803,	0.014812,	0.021385,	0.02417,	0.021385,	0.014812,	0.00803,	0.003408,	0.001132,
    0.001634,	0.00492,	0.011594,	0.021385,	0.030875,	0.034896,	0.030875,	0.021385,	0.011594,	0.00492,	0.001634,
    0.001847,	0.005561,	0.013104,	0.02417,	0.034896,	0.03944,	0.034896,	0.02417,	0.013104,	0.005561,	0.001847,
    0.001634,	0.00492,	0.011594,	0.021385,	0.030875,	0.034896,	0.030875,	0.021385,	0.011594,	0.00492,	0.001634,
    0.001132,	0.003408,	0.00803,	0.014812,	0.021385,	0.02417,	0.021385,	0.014812,	0.00803,	0.003408,	0.001132,
    0.000614,	0.001848,	0.004354,	0.00803,	0.011594,	0.013104,	0.011594,	0.00803,	0.004354,	0.001848,	0.000614,
    0.00026,	0.000784,	0.001848,	0.003408,	0.00492,	0.005561,	0.00492,	0.003408,	0.001848,	0.000784,	0.00026,
    0.000086,	0.00026,	0.000614,	0.001132,	0.001634,	0.001847,	0.001634,	0.001132,	0.000614,	0.00026,	0.000086,

};

float getDepth(vec2 pos){
    return length(texture(gPosition,TexCoord).rgb - viewPos);
}
/*
vec4 RayCast(in vec3 dir, inout vec3 hitCoord, out float dDepth){
    dir *= rayStep;
    float depth = 0.0;
    int steps = 0;
    vec4 projectedCoord = vec4(0.0);

    for(int i = 0; i < maxSteps; ++i){
        hitCoord += dir;
        projectedCoord = proj * view * vec4(hitCoord,1.0);
        //projectedCoord.xy /= projectedCoord.w;
        //projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        depth = texture(gDepth, projectedCoord.xy).z * 25;
        if(depth > 1000.0)
            continue;

            dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2)
            if(dDepth <= 0){
                vec4 Result = vec4(projectedCoord.xy,dDepth,1.0);
                return Result;
            }
    }
    return vec4(-1);
}
vec4 RayCast(in vec3 dir, inout vec3 hitCoord, out float dDepth){
    dir *= rayStep;
    float depth = 0.0;
    int steps = 0;
    vec4 projectedCoord = vec4(0.0);
    vec4 resCord = vec4(-1);
    return resCord;
    for(int i = 0; i < maxSteps; ++i){
        hitCoord += dir;
        projectedCoord =  proj * view * vec4(hitCoord,1.0);
        projectedCoord.z -= 0.1;
        
        float visible = texture(gDepth,projectedCoord.xyz).r;

        if(visible > 0.1){
            resCord = projectedCoord;
        }else{
            return resCord;
        }
    }
    
    return resCord;
}
*/
float linearDepth(float depthSample)
{
   return (2 * near) / (far + near - depthSample * (far - near));
}
vec4 RayCast(in vec3 dir, inout vec3 hitCoord, out float dDepth){
    dir = normalize(dir) * rayStep;
    vec4 projectedCoord = vec4(0);
    float currentDepth = 0;
    vec3 lastSuccessPos;
    for(int i = 0; i < maxSteps; ++i){
        //hitCoord += dir;
        projectedCoord = proj * view * vec4(hitCoord,1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
        dDepth = length(hitCoord - viewPos) / 10;
        projectedCoord.z = 10;
        float visible = texture(gDepth,projectedCoord.xyz).r;
        return vec4(visible);

        if(currentDepth > projectedCoord.z){
            lastSuccessPos = hitCoord;
        }else{
            break;
        }
    }
    hitCoord = lastSuccessPos;
    return vec4(hitCoord,1);
}
vec3 Blurr(vec2 pos, float strength){
    vec3 outColor = vec3(0);
    vec2 pixelsize = vec2( 1.0 / float(screenX), 1.0 / float(screenY));

    const int samplesize = 11;
    float pxx = pixelsize.x;
    float pxy = pixelsize.y;
    strength = strength;
    vec2 pxs = vec2(pxx, pxy);

    for(int i = 0; i < samplesize; ++i){
        int x = i - ((samplesize - 1) / 2);
        for(int j = 0; j < samplesize; ++j){
            int y = j - ((samplesize - 1) / 2);

            vec2 tcoord = vec2(x * pxx * strength,y * pxy *strength);
            vec2 tc = pos + tcoord;
            vec3 fcol = texture(gColor,tc).rgb;
            int sp = (i * 11 + j);
            outColor += fcol * hqweights[sp];
        }
    }
    return outColor;
}
vec3 UI(vec3 bgcol){
    int l0 = -1;
        vec2 l0tc = vec2(0);

    int l1 = -1;
        vec2 l1tc = vec2(0);

    int l2 = -1;
        vec2 l2tc = vec2(0);


    for(int i=0; i < int(uie.length()); i++){
        float px = (uie[i].posisionPercent.x / 100) + (uie[i].positionPixel.x / screenX); 
        float py = (uie[i].posisionPercent.y / 100) + (uie[i].positionPixel.y / screenY); 
        float sx = (uie[i].sizePercent.x / 100) + (uie[i].sizePixel.x / screenX); 
        float sy = (uie[i].sizePercent.y / 100) + (uie[i].sizePixel.y / screenY); 
        float pix = (TexCoord.x - px) / sx;
        float piy = (TexCoord.y - py) / sy;

        if(pix > 0 && pix < 1 && piy > 0 && piy < 1 ){
            if(uie[i].z > uie[l0].z){
                l2 = l1;
                l1 = l0;
                l0 = i;

                l2tc = l1tc;
                l1tc = l0tc;
                l0tc = vec2(pix,piy);

            }else if(uie[i].z > uie[l1].z){
                l2 = l1;
                l1 = i;

                l2tc = l1tc;
                l1tc = vec2(pix,piy);
            }else if(uie[i].z > uie[l2].z){
                l2 = i;

                l2tc = vec2(pix,piy);
            }
        }
    }
    vec3 fgbg = bgcol;
    vec3 oc = bgcol;
    if(l0 > -1){
        vec3 tex = texture(textures,vec3(l0tc,uie[l0].texture)).rgb;
        float alpha;
        if(uie[l0].alphamap == 0){
            alpha = 1;
        }else{
            alpha = 1 - texture(textures,vec3(l0tc,uie[l0].alphamap)).r;
        }
        fgbg = uie[l0].foregroundColor * tex.rgb * alpha + uie[l0].backgroundColor * (1 - alpha);

        if(uie[l0].backgoundBlurr == 0){
            if(l1 != -1){
                if(uie[l0].opacity != 1 && uie[l1].backgoundBlurr != 0){
                    bgcol = Blurr(TexCoord,uie[l1].backgoundBlurr);
                }
            }
        }else{
            bgcol = Blurr(TexCoord,uie[l0].backgoundBlurr);
        }
        if(l1 != -1 && uie[l0].opacity >= 1){
            vec3 tex1 = texture(textures,vec3(l0tc,uie[l0].texture)).rgb;
            float alpha1;
            if(uie[l1].alphamap == 0){
                alpha1 = 1;
            }else{
                alpha1 = 1 - texture(textures,vec3(l0tc,uie[l0].alphamap)).r;
            }
            vec3 fgbg1 = uie[l1].foregroundColor * tex1.rgb * alpha1 + uie[l1].backgroundColor * (1 - alpha1);
            oc = fgbg * (uie[l0].opacity * alpha) + fgbg1 * (uie[l1].opacity * alpha1) + bgcol * (1 - (uie[l1].opacity * alpha1)) * (1 - (uie[l0].opacity * alpha));

        }else{
            oc = fgbg * (uie[l0].opacity * alpha)+ bgcol * (1 - (uie[l0].opacity * alpha));

        }
        
    }
    return oc;
}
void main(){
    float roughness = texture(gMaterial, TexCoord).r;
    float metallic = texture(gMaterial, TexCoord).g;
    vec3 viewNormal = vec3(texture(gNormal, TexCoord));
    vec3 globalPos =  vec3(texture(gPosition, TexCoord));
    vec3 color = texture(gColor,TexCoord).rgb;

    //if(roughness < 100.8 && useSSR){
        float dX = (2 * TexCoord.x ) - 1;
        float dY = (2 * TexCoord.y ) - 1; 
        vec4 dir = invView * vec4((invProj * vec4(dX,dY,0.1,1)).xyz,0);

        vec3 hitCoord = globalPos;
        float dist;
        vec3 direction = normalize(reflect(normalize(dir.xyz),normalize(viewNormal)));
        vec4 hitpos = RayCast(direction,hitCoord, dist);
        vec3 hitcol =  texture(gColor,hitpos.xy).rgb;
        FragColor = hitpos;
        //FragColor = vec4(hitcol,1);
        color += 0.5 * hitcol;
    //}
    color = UI(color);
    //FragColor = vec4(color,1);

    //FragColor = vec4(Blurr(TexCoord,1),0);
}
