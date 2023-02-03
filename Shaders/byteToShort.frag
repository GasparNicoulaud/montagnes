#version 150

uniform usampler2D bytes;

out uvec4 out_Color;

void main(void)
{
    uint byte = texture(bytes, gl_FragCoord.xy/2048.0f).r;
    uint outc = 0U;
    if(byte == 1U) // minor
    {
        outc = 4096U; // 0 bit then 1 bit and 14 * 0b bits ( 0100000000000000)
    }
    else if(byte == 3U) // major
    {
        outc = 8192U; // 1 bit and 14 * 0b bits ( 1000000000000000)
    }
    else if(byte == 9U) // initial
    {
        outc = 16000U + 12288U; //  16000d + 2 bits (11) at the start
    }
    
    //outc = uint(ceil(sin(gl_FragCoord.x / 300.0f) * 3000));
    
    out_Color = uvec4(outc);
}

/*



padding



*/
