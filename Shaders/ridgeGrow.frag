#version 410 core

out uvec4 out_Color;
uniform usampler2D sampler;
uniform float time;

float randO(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898 + time,78.233 - time))) * 43758.5453);
}

float rand2(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(rand(co.xy) * 100,rand(co.yx) * 100))) * 438.53);
}

void main(void)
{
    float turnOnProb = 0;
    float globPopProb = 0.1;
    
    //float output;
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
     
     if(nbCount == 1)
     {
         turnOnProb += 0.1 * globPopProb;
     }
    if(nbCount == 2 && rand2(gl_FragCoord.xy * time *3) > 0.999)
    {
        turnOnProb += 0.001 * globPopProb;
    }
    
    
     uint nbCountR2 = 0;
     
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,2)).r; // top
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(0,2)).r; // top
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(1,2)).r; // top
     
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,-1)).r; // right
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,0)).r; // right
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,1)).r; // right
     
     
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-2)).r; // bot
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(0,-2)).r; // bot
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(1,-2)).r; // bot
     
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-1)).r; // left
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,0)).r; // left
     nbCountR2 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,1)).r; // left
     
     if(nbCountR2 >= 4)
     {
     turnOnProb -= 0.1* globPopProb;
     }
     
     uint nbCountR3 = 0;
    
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,3)).r; // top
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(0,3)).r; // top
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(1,3)).r; // top
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,2)).r; // top
    
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,1)).r; // right
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,0)).r; // right
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,-1)).r; // right
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,-2)).r; // right
    
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-3)).r; // top
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(0,-3)).r; // top
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(1,-3)).r; // top
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-2)).r; // top
    
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-1)).r; // right
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,0)).r; // right
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,1)).r; // right
     nbCountR3 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,2)).r; // right
     
     if(nbCountR3 >= 4)
     {
     turnOnProb -= 0.1* globPopProb;
     }
    
    uint nbCountR4 = 0;
    
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,3)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,3)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,4)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-0,4)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(1,4)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,3)).r; // top
    //nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,3)).r; // top
    
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,-3)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,-2)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(4,1)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(4,0)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(4,1)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,2)).r; // right
    //nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,3)).r; // right
    
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,3)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-3)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-4)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-0,-4)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(1,-4)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,-3)).r; // top
    //nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,-3)).r; // top
    
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-3)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-2)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,1)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,0)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,1)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,2)).r; // right
    //nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,3)).r; // right
    
    if(nbCountR4 >= 4)
    {
        turnOnProb -= 0.1* globPopProb;
    }

    
    uint nbCountR5 = 0;
    
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,4)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,4)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,5)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-0,5)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(1,5)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,4)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,4)).r; // top
    
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(4,-3)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(4,-2)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(5,1)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(5,0)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(5,1)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(4,2)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(4,3)).r; // right
    
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-3,-4)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-2,-4)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-1,-5)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-0,-5)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(1,-5)).r; // top
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(2,-4)).r; // top
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(3,-4)).r; // top
    
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-3)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,-2)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,1)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,0)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-5,1)).r; // right
    nbCountR5 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,2)).r; // right
    nbCountR4 += texelFetchOffset(sampler, fragCoord, 0, ivec2(-4,3)).r; // right
    
    if(nbCountR5 >= 4)
    {
        turnOnProb -= 0.1* globPopProb;
    }

    
    turnOnProb = clamp(turnOnProb,0,1);
    
    
    uint us = texelFetch(sampler, fragCoord, 0).r;
    if(us > 0.5)
    {
        turnOnProb = 1;
    }
    
    float fout = clamp(ceil(turnOnProb - rand2(gl_FragCoord.xy * (time + 1)) ) ,0,1);
    out_Color = uvec4(uint(fout));
}







