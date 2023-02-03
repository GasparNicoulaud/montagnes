#version 150 core

out vec4 out_Color;

uniform sampler2D sampler;

void main(void)
{
    vec4 outc = texture(sampler,gl_FragCoord.xy/2048.0f).rgba;
    out_Color = outc;
    out_Color.w = 1;
}


