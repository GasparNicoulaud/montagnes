#version 150

uniform usampler2D majorRidge;
uniform usampler2D sampler;
out vec4 out_Color;

float sampleMinor(ivec2 fragCoord)
{
    const int maxrad = 50;
    float distFromClosestRidge = maxrad;
    uint ridgeHit;
    
    for(int x = maxrad * -1; x < maxrad; x++)
    {
        for(int y = maxrad * -1; y < maxrad; y++)
        {
            ridgeHit = texelFetch(sampler, ivec2((fragCoord + ivec2(x,y))/8), 0).r;
            
            if(ridgeHit == 1U)
            {
                distFromClosestRidge = min(distFromClosestRidge, length(ivec2(x,y)));
            }
        }
    }
    
    return distFromClosestRidge;
}

float sampleMajor(ivec2 fragCoord)
{
    const int maxrad = 80;
    float distFromClosestRidge = maxrad;
    uint ridgeHit;
    
    for(int x = maxrad * -1; x < maxrad; x++)
    {
        for(int y = maxrad * -1; y < maxrad; y++)
        {
            ridgeHit = texelFetch(majorRidge, ivec2((fragCoord + ivec2(x,y))/8), 0).r;
            
            if(ridgeHit == 1U)
            {
                distFromClosestRidge = min(distFromClosestRidge, length(ivec2(x,y)));
            }
        }
    }
    
    return distFromClosestRidge;
}


void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    
    float valFromMinor = sampleMinor(fragCoord);
    float valFromMajor = sampleMajor(fragCoord);
    
    float outc = ((100.0 - valFromMinor) + (100.0 - valFromMajor)) * 0.01;
    
    ///*
    outc = 0;
    
    uint val = texture(sampler, gl_FragCoord.xy/2048.0f).r;
    if(val == 0U)
    {
        outc = 0;
    }
    else if(val == 1U)
    {
        outc = 1;
    }
    else if(val == 2U)
    {
        outc = 0.2;
    }
    else if(val == 4U)
    {
        outc = 0.1;
    }
    
    if(texture(majorRidge, gl_FragCoord.xy/2048.0f).r == 1U)
    {
        outc = 3;
    }
    
    outc = outc * 0.1;
    //*/
    
    out_Color = vec4(outc,0,0,1);
}




