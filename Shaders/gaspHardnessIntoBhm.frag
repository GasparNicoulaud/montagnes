#version 150 core

uniform usampler2D hardness;
uniform sampler2D bhm; //bhmtmp
out vec4 out_Color; //bhm


void main(void)
{
    uint hardness = texture(hardness,gl_FragCoord.xy/2048.0f).r;
    vec4 bhm = texture(bhm,gl_FragCoord.xy/2048.0f).rgba;
    
    bhm.b = float(hardness)/255.0f;
    
    out_Color = bhm;
}




