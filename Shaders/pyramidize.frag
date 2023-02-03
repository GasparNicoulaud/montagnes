#version 150

uniform usampler2D sampler;
out uvec4 out_Color;

float sampleArea(ivec2 fragCoord)
{
    //int maxrad = 10;
    
    float maxHeight = 0;
    
    uint ridgeHit;
    
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,-2)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,-1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,0)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,0)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-2,2)) * 25));
    
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,-2)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,-1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,0)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,0)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(-1,2)) * 25));
    
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(0,-2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,-2)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(0,-1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,-1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(0,0)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,0)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(0,1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(0,2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(0,2)) * 25));
    
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(1,-2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,-2)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(1,-1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,-1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(1,0)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,0)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(1,1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(1,2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(1,2)) * 25));
    
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(2,-2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,-2)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(2,-1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,-1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(2,0)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,0)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(2,1)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,1)) * 25));
    ridgeHit = texelFetchOffset(sampler, fragCoord, 0, ivec2(2,2)).r;
    maxHeight = max(maxHeight, ridgeHit - (length(ivec2(2,2)) * 25));
    
    
    
    return maxHeight;
}

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    
    float outc = sampleArea(fragCoord);
    
    out_Color = uvec4(outc,0,0,1);
}




