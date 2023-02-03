#version 410 core

out uvec4 out_Color;
uniform usampler2D sampler;

void main(void)
{
    ivec2 fragCoord = ivec2(gl_FragCoord.xy); //cast to int
    const ivec3 zeroOneMinusOne = ivec3(0,1,-1);
    
    uint nbCount = 0;
    
    nbCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xy).r; // top
    nbCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yx).r; // right
    nbCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.xz).r; // bottom
    nbCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zx).r; // left
    nbCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yy).r; // top-right
    nbCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zy).r; // bottom-right
    nbCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.zz).r; // bottom-left
    nbCount += texelFetchOffset(sampler, fragCoord, 0, zeroOneMinusOne.yz).r; // top-left
    
    uint finalOut = texelFetch(sampler, fragCoord, 0).r;
    
    if(nbCount <= 1U)
    {
        finalOut = 0U;
    }
    
    out_Color = uvec4(finalOut);
}



