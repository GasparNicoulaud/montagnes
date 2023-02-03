
#version 150 core

out vec4 out_Color;

uniform sampler2D sampler;

float sampleOfst(vec2 ofst)
{
    return texture(sampler,(gl_FragCoord.xy + ofst)/2048.f).x; //2048 because hardcoded fbo res is 2048x2048
}

void main(void)
{
float tl, tm, tr, ml, mm, mr, bl, bm, br;

float finalGain = 0;

//check neigbourhs
tl = texture(sampler,vec2((gl_FragCoord.x - 1)/2048.0,(gl_FragCoord.y + 1)/2048.0)).x; //2048 because hardcoded fbo res is 2048x2048
tm = texture(sampler,vec2((gl_FragCoord.x    )/2048.0,(gl_FragCoord.y + 1)/2048.0)).x;
tr = texture(sampler,vec2((gl_FragCoord.x + 1)/2048.0,(gl_FragCoord.y + 1)/2048.0)).x;

ml = texture(sampler,vec2((gl_FragCoord.x - 1)/2048.0,(gl_FragCoord.y    )/2048.0)).x;
mm = texture(sampler,vec2((gl_FragCoord.x    )/2048.0,(gl_FragCoord.y    )/2048.0)).x;
mr = texture(sampler,vec2((gl_FragCoord.x + 1)/2048.0,(gl_FragCoord.y    )/2048.0)).x;

bl = texture(sampler,vec2((gl_FragCoord.x - 1)/2048.0,(gl_FragCoord.y - 1)/2048.0)).x;
bm = texture(sampler,vec2((gl_FragCoord.x    )/2048.0,(gl_FragCoord.y - 1)/2048.0)).x;
br = texture(sampler,vec2((gl_FragCoord.x + 1)/2048.0,(gl_FragCoord.y - 1)/2048.0)).x;
    
    float blurredValue = 41 * mm + 26 * (tm + ml + mr + bm) + 16 * (tl + tr + bl + br);
    
    //float blurredValue = 0.5 * mm + 0.25 * (tm + ml + mr + bm) + 0.125 * (tl + tr + bl + br);
    
    blurredValue += 1 * (sampleOfst(vec2(-2,2)) + sampleOfst(vec2(2,2)) + sampleOfst(vec2(2,-2)) + sampleOfst(vec2(-2,-2)));
    blurredValue += 4 * (sampleOfst(vec2(2,1)) + sampleOfst(vec2(2,-1)) + sampleOfst(vec2(-2,1)) + sampleOfst(vec2(-2,-1)) + sampleOfst(vec2(1,2)) + sampleOfst(vec2(-1,2)) + sampleOfst(vec2(1,-2)) + sampleOfst(vec2(-1,-2)));
    blurredValue += 7 * (sampleOfst(vec2(0,2)) + sampleOfst(vec2(0,-2)) + sampleOfst(vec2(2,0)) + sampleOfst(vec2(-2,0)));
    
    blurredValue *= 0.003663003663;
    //blurredValue *= 0.5;
    
    float outc = max(mm,blurredValue);
    //outc += 0.1;
    
    out_Color = vec4(outc,max(0.0,blurredValue - mm) * 100 + texture(sampler,gl_FragCoord.xy/2048.0f).y,0,1);
}

