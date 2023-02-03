#version 150 core

uniform usampler2D flowMap;
uniform sampler2D bhm;

out vec4 out_Color;

void main(void)
{
    uint flowTrace = texture(flowMap,gl_FragCoord.xy/2048.0f).y;
    vec4 bhm = texture(bhm,gl_FragCoord.xy/2048.0f);
    
    bhm.r *= 1.5; //final amp control for heightmap
    bhm.z = float(flowTrace);
    
    out_Color = bhm;
}


