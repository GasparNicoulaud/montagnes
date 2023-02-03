#version 150 core

uniform usampler2D sampler;
out vec4 out_Color;

void main(void)
{
    float outc, sed;
    uvec4 sampleres = texture(sampler,gl_FragCoord.xy/2048.0f);
    sed = clamp(sampleres.g,0U,8000U);
    //sed = float(sampleres.g) / 5.0;
    
    outc = ((float(sampleres.r) + sed )/ 10000.0f) - 2;
    
    out_Color = vec4(outc,sed * 2,0,1);
}

