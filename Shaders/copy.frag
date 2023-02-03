
#version 150 core

out vec4 out_Color;

uniform sampler2D sampler;

in float inoutmode;

void main(void)
{
    vec4 outc = texture(sampler,gl_FragCoord.xy/2048.0f).rgba;
    //float sed = texture(sampler,gl_FragCoord.xy/2048.0f).y;
    //if(sed >0 && sed < 5)
    //{
        //outc += sed;
    //}
    out_Color = vec4(outc.x,0,0,outc.w);
    
    out_Color.w = 1;
}

