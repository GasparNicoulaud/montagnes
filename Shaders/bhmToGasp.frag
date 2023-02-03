#version 150 core

uniform sampler2D bhm;
uniform usampler2D hardness;
out uvec4 out_Color;


void main(void)
{
    uint hardness = texture(hardness,gl_FragCoord.xy/2048.0f).r;
    
    vec4 terrain = texture(bhm,gl_FragCoord.xy/2048.0f).rgba;
    terrain.r += 2;
    terrain.r *= 10000;
    //out_Color = uvec4(uint(terrain.r),hardness,0U,0U);
    out_Color = uvec4(uint(terrain.r),hardness,0U,0U);
}


