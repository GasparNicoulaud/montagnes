#version 150

uniform usampler2D sampler;
out uvec4 out_Color;

float sampleArea(ivec2 fragCoord)
{
    int maxrad = 2;
    
    float maxHeight = 0;
    
    uint ridgeHit;
    
    float slope = 0;
    
    uint us = texture(sampler,gl_FragCoord.xy/2048.0f).r;
    
    if((us & 12288U) != 0U)
    {
        if((us & 12288U) == 12288U) //ORIGINAL RIDGE
        {
            slope = 0;
        }
        else if((us & 12288U) == 4096U) //minor ridge
        {
            slope = 4;
        }
        else if((us & 12288U) == 8192U) // major ridge
        {
            slope = 2;
        }
        
        for(int x = maxrad * -1; x < maxrad; x++)
        {
            for(int y = maxrad * -1; y < maxrad; y++)
            {
                ridgeHit = texelFetch(sampler, ivec2((fragCoord + ivec2(x,y))), 0).r;
            
                maxHeight = max(maxHeight, ridgeHit - (length(ivec2(x,y)) * slope));
            }
        }
    }
    
    return maxHeight;
}

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    
    float outc = sampleArea(fragCoord);
    
    out_Color = uvec4(outc,0,0,1);
}




