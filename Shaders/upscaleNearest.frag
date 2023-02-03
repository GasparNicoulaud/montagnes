#version 150

uniform usampler2D majorRidge;
uniform usampler2D sampler;

out uvec4 out_Color;

void main(void)
{
    uint outc = 0U;
    if(texture(sampler, gl_FragCoord.xy/2048.0f).r == 1U)
    {
        outc = 1U;
    }
    if(texture(majorRidge, gl_FragCoord.xy/2048.0f).r == 1U)
    {
        outc = 3U;
    }
    if(texture(majorRidge, gl_FragCoord.xy/2048.0f).r == 9U)
    {
        outc = 9U;
    }
    
    out_Color = uvec4(outc);
}




