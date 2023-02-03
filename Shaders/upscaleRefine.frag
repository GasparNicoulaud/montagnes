#version 410 core

out uvec4 out_Color;
uniform usampler2D sampler;


void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    
    uint us = texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xx).r;
    
    
    uint uoutput = 0U;
    
    if((us & 1U) == 1U) //ridge
    {
        uint neighboursCount = 0U; //neighbors that are ridges
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xy).r & 1U; // top
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yx).r & 1U; // right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xz).r & 1U; // bottom
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zx).r & 1U; // left
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yy).r & 1U; // top-right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zy).r & 1U; // bottom-right
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zz).r & 1U; // bottom-left
        neighboursCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yz).r & 1U; // top-left
        
        if(neighboursCount == 2U || neighboursCount == 4U || neighboursCount == 6U || neighboursCount == 8U)
        {
            uoutput = us;
        }
    }
    
    out_Color = uvec4(uoutput);
}



